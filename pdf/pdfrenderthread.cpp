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
    Private() : document{ nullptr }, tocModel{ nullptr }, canvasWidth(0) { }

    QThread* thread;
    QTimer* updateTimer;
    QQueue< PDFJob* > jobQueue;

    QMutex mutex;

    Poppler::Document* document;
    PDFTocModel* tocModel;
    QObjectList linkTargets;
    uint canvasWidth;

    void rescanDocumentLinks()
    {
        qDeleteAll(linkTargets);
        linkTargets.clear();
        qreal pageTop = 0;
        qreal sizeAdjustment = (qreal)canvasWidth / document->page(0)->pageSizeF().width();
        for(int i = 0; i < document->numPages(); ++i)
        {
            Poppler::Page* page = document->page(i);
            foreach(Poppler::Link* link, page->links())
            {
                if(link->linkType() == Poppler::Link::Browse)
                {
                    Poppler::LinkBrowse* realLink = static_cast<Poppler::LinkBrowse*>(link);
                    QRectF linkPos(pageTop + page->pageSizeF().height() * link->linkArea().top() * sizeAdjustment - (page->pageSizeF().height() * -link->linkArea().height() * sizeAdjustment * 0.5),
                                   page->pageSizeF().width() * link->linkArea().left() * sizeAdjustment,
                                   page->pageSizeF().height() * -link->linkArea().height() * sizeAdjustment,
                                   page->pageSizeF().width() * link->linkArea().width() * sizeAdjustment);
                    QObject * obj = new QObject(updateTimer);
                    obj->setProperty("linkRect", linkPos);
                    obj->setProperty("linkTarget", realLink->url());
                    linkTargets.append(obj);
                }
            }
            pageTop += (page->pageSizeF().height() * sizeAdjustment) + 0.025 * (page->pageSizeF().height() * sizeAdjustment);
        }
    }
};

PDFRenderThread::PDFRenderThread(QObject* parent)
    : QObject( parent ), d( new Private() )
{
    d->thread = new QThread(this);

    d->updateTimer = new QTimer();
    d->updateTimer->setInterval(500);
    d->updateTimer->start();
    connect(d->updateTimer, SIGNAL(timeout()), this, SLOT(processQueue()), Qt::DirectConnection);
    d->updateTimer->moveToThread(d->thread);

    d->thread->start(QThread::LowestPriority);
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

QObjectList PDFRenderThread::linkTargets() const
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

void PDFRenderThread::setCanvasWidth(uint width)
{
    d->canvasWidth = width;
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
