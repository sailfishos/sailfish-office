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

#include <poppler-qt4.h>

#include "pdfjob.h"
#include "pdftocmodel.h"

PDFRenderThread* PDFRenderThread::sm_instance = 0;

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
};

PDFRenderThread::PDFRenderThread(QObject* parent)
    : QObject( parent ), d( new Private() )
{
    d->thread = new QThread(this);

    d->updateTimer = new QTimer();
    d->updateTimer->setInterval(100);
    d->updateTimer->start();
    connect(d->updateTimer, SIGNAL(timeout()), this, SLOT(processQueue()), Qt::DirectConnection);
    d->updateTimer->moveToThread(d->thread);

    d->thread->start(QThread::LowPriority);
}

PDFRenderThread::~PDFRenderThread()
{
    d->thread->exit();
    d->thread->wait();

    qDeleteAll(d->jobQueue);

    delete d->updateTimer;
    delete d->document;

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

void PDFRenderThread::load(const QString& file)
{
    LoadDocumentJob* job = new LoadDocumentJob{ file };
    job->moveToThread( d->thread );

    QMutexLocker locker{ &d->mutex };
    d->jobQueue.enqueue( job );
}

void PDFRenderThread::requestPage(int index, uint width )
{
    RenderPageJob* job = new RenderPageJob{ index, width, d->document };
    job->moveToThread( d->thread );

    QMutexLocker locker{ &d->mutex };
    d->jobQueue.enqueue( job );
}

PDFRenderThread* PDFRenderThread::instance()
{
    if( !sm_instance )
        sm_instance = new PDFRenderThread( QCoreApplication::instance() );

    return sm_instance;
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
            emit loadFinished();
            break;
        }
        case PDFJob::RenderPageJob: {
            RenderPageJob* rj = static_cast< RenderPageJob* >( job );
            emit pageFinished( rj->m_index, rj->m_page );
        }
    }

    job->deleteLater();
}
