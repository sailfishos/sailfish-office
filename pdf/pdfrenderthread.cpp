/*
 *
 */

#include "pdfrenderthread.h"

#include <QThread>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QDebug>
#include <QCoreApplication>

#include <poppler-qt5.h>

#include "pdfjob.h"
#include "pdftocmodel.h"

class PDFRenderThread::Private
{
public:
    Private() : document{ nullptr }, tocModel{ nullptr } { }

    QThread* thread;
    QTimer* updateTimer;
    QQueue< PDFJob* > jobQueue;

    QMutex mutex;

    Poppler::Document* document;
    PDFTocModel* tocModel;

    QMultiMap< int, QPair< QRectF, QUrl > > linkTargets;

    void rescanDocumentLinks()
    {
        linkTargets.clear();

        for(int i = 0; i < document->numPages(); ++i)
        {
            Poppler::Page* page = document->page(i);
            QSizeF pageSize = page->pageSizeF();
            for(Poppler::Link* link : page->links())
            {
                if(link->linkType() == Poppler::Link::Browse)
                {
                    Poppler::LinkBrowse* realLink = static_cast<Poppler::LinkBrowse*>(link);
                    QRectF linkArea = link->linkArea();
                    linkTargets.insert( i, QPair< QRectF, QUrl >{ linkArea, realLink->url() } );
                }
            }
        }
    }
};

PDFRenderThread::PDFRenderThread(QObject* parent)
    : QObject( parent ), d( new Private() )
{
    d->thread = new QThread(this);

    d->updateTimer = new QTimer();
    d->updateTimer->setInterval(50);
    d->updateTimer->start();
    connect(d->updateTimer, SIGNAL(timeout()), this, SLOT(processQueue()), Qt::DirectConnection);
    d->updateTimer->moveToThread(d->thread);

    d->thread->start();
}

PDFRenderThread::~PDFRenderThread()
{
    d->thread->exit();
    d->thread->wait();

    qDeleteAll(d->jobQueue);

    delete d->updateTimer;
    delete d->document;
    delete d->tocModel;

    delete d;
}

int PDFRenderThread::pageCount() const
{
    QMutexLocker locker{ &d->mutex };
    return d->document->numPages();
}

QObject* PDFRenderThread::tocModel() const
{
    return d->tocModel;
}

bool PDFRenderThread::isLoaded() const
{
    return d->document != nullptr;
}

QMultiMap< int, QPair< QRectF, QUrl > > PDFRenderThread::linkTargets() const
{
    return d->linkTargets;
}

void PDFRenderThread::queueJob(PDFJob* job)
{
    job->m_document = d->document;
    job->moveToThread( d->thread );

    QMutexLocker locker{ &d->mutex };
    d->jobQueue.enqueue( job );
}

void PDFRenderThread::processQueue()
{
    QMutexLocker locker{ &d->mutex };

    if( d->jobQueue.count() == 0 )
        return;

    PDFJob* job = d->jobQueue.dequeue();
    job->run();

    switch(job->type())
    {
        case PDFJob::LoadDocumentJob: {
            LoadDocumentJob* dj = static_cast< LoadDocumentJob* >( job );
            if( d->document )
                delete d->document;

            d->document = dj->m_document;
            if(d->tocModel) {
                d->tocModel->deleteLater();
                d->tocModel = nullptr;
            }
            d->tocModel = new PDFTocModel{ d->document };
            d->rescanDocumentLinks();
            job->deleteLater();
            emit loadFinished();
            break;
        }
        default:
            emit jobFinished(job);
    }
}
