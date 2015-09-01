/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Contact: Robin Burchell <robin.burchell@jolla.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "pdfrenderthread.h"

#include <QThread>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QDebug>
#include <QCoreApplication>
#include <QUrlQuery>

#include <poppler-qt5.h>

#include "pdfjob.h"
#include "pdftocmodel.h"

class PDFRenderThreadQueue;

const QEvent::Type Event_JobPending = QEvent::Type(QEvent::User + 1);

class Thread : public QThread
{
    Q_OBJECT;
public:
    Thread()
        : jobQueue(0)
    {
    }

    void run() {
        QThread::exec();
        delete document;
        delete tocModel;
        deleteLater();
    }

    bool event(QEvent *e) {
        // intercept deleteLater to wait for this thread to be
        // finished. CAlled on the GUI thread.
        if (e->type() == QEvent::DeferredDelete)
            wait();
        return QThread::event(e);
    }

    PDFRenderThreadQueue *jobQueue;
    QMutex mutex;

    // Used for cleanup only
    Poppler::Document *document;
    PDFTocModel *tocModel;


};

class PDFRenderThreadPrivate
{
public:
    PDFRenderThreadPrivate() : document{ nullptr }, tocModel{ nullptr } { }

    PDFRenderThread *q;

    Thread *thread;

    bool loadFailure;
    Poppler::Document* document;
    PDFTocModel* tocModel;

    QMultiMap< int, QPair< QRectF, QUrl > > linkTargets;

    void rescanDocumentLinks()
    {
        linkTargets.clear();

        for(int i = 0; i < document->numPages(); ++i)
        {
            Poppler::Page* page = document->page(i);
            for(Poppler::Link* link : page->links())
            {
                switch (link->linkType()) {
                case (Poppler::Link::Browse): {
                    Poppler::LinkBrowse* realLink = static_cast<Poppler::LinkBrowse*>(link);
                    QRectF linkArea = link->linkArea();
                    linkTargets.insert( i, QPair< QRectF, QUrl >{ linkArea, realLink->url() } );
                    break;
                }
                case (Poppler::Link::Goto): {
                    Poppler::LinkGoto* gotoLink = static_cast<Poppler::LinkGoto*>(link);
                    // Not handling goto link to external file currently.
                    if (gotoLink->isExternal())
                        break;
                    QRectF linkArea = link->linkArea();
                    QUrl linkURL = QUrl("");
                    QUrlQuery query = QUrlQuery();
                    query.addQueryItem("page", QString::number(gotoLink->destination().pageNumber()));
                    if (gotoLink->destination().isChangeLeft()) {
                        query.addQueryItem("left", QString::number(gotoLink->destination().left()));
                    }
                    if (gotoLink->destination().isChangeTop()) {
                        query.addQueryItem("top", QString::number(gotoLink->destination().top()));
                    }
                    linkURL.setQuery(query);
                    linkTargets.insert( i, QPair< QRectF, QUrl >{ linkArea, linkURL } );
                    break;
                }
                default:
                    break;
                }

            }
        }
    }

};

class PDFRenderThreadQueue : public QObject, public QQueue< PDFJob* >
{
public:
    PDFRenderThreadPrivate *d;
protected:
    bool event(QEvent *);
    void processPendingJob();
};

PDFRenderThread::PDFRenderThread(QObject* parent)
    : QObject( parent ), d( new PDFRenderThreadPrivate() )
{
    d->q = this;
    d->loadFailure = false;
    d->thread = new Thread();
    d->thread->jobQueue = new PDFRenderThreadQueue();
    d->thread->jobQueue->d = d;
    d->thread->start();
    d->thread->jobQueue->moveToThread(d->thread);
}

PDFRenderThread::~PDFRenderThread()
{
    // Cancel outstanding render jobs and schedule the queue
    // for deletion. Also set the jobQueue to 0 so we don't
    // end up calling back to the now deleted documents object.
    cancelRenderJob(-1);
    d->thread->mutex.lock();
    d->thread->document = d->document;
    d->thread->tocModel = d->tocModel;
    d->thread->jobQueue->deleteLater();
    d->thread->jobQueue = 0;
    d->thread->mutex.unlock();
    d->thread->exit();

    delete d;
}

int PDFRenderThread::pageCount() const
{
    QMutexLocker locker{ &d->thread->mutex };
    if (d->document != nullptr &&
        !d->document->isLocked()) {
        return d->document->numPages();
    } else {
        return 0;
    }
}

QObject* PDFRenderThread::tocModel() const
{
    QMutexLocker{ &d->thread->mutex };
    return d->tocModel;
}

bool PDFRenderThread::isLoaded() const
{
    QMutexLocker{ &d->thread->mutex };
    return d->document != nullptr;
}
bool PDFRenderThread::isFailed() const
{
    QMutexLocker{ &d->thread->mutex };
    return d->loadFailure;
}
bool PDFRenderThread::isLocked() const
{
    QMutexLocker(&d->thread->mutex);
    return (d->document != nullptr) ? d->document->isLocked() : false;
}

QMultiMap< int, QPair< QRectF, QUrl > > PDFRenderThread::linkTargets() const
{
    QMutexLocker{ &d->thread->mutex };
    return d->linkTargets;
}

void PDFRenderThread::queueJob(PDFJob* job)
{
    QMutexLocker locker{ &d->thread->mutex };
    job->moveToThread( d->thread );
    d->thread->jobQueue->enqueue( job );
    QCoreApplication::postEvent(d->thread->jobQueue, new QEvent(Event_JobPending));
}

void PDFRenderThread::cancelRenderJob(int index)
{
    QMutexLocker locker{ &d->thread->mutex };
    for (QList<PDFJob *>::iterator it = d->thread->jobQueue->begin(); it != d->thread->jobQueue->end(); ) {
        PDFJob *j = *it;
        if (j->type() == PDFJob::RenderPageJob
                && (index < 0 || static_cast<RenderPageJob *>(j)->m_index == index)) {
            it = d->thread->jobQueue->erase(it);
            j->deleteLater();
            continue; // to skip the ++it at the end of the loop
        }
        ++it;
    }
}

void PDFRenderThread::prioritizeJob(int index, int size)
{
    QMutexLocker locker{ &d->thread->mutex };
    for (QList<PDFJob *>::iterator it = d->thread->jobQueue->begin(); it != d->thread->jobQueue->end(); ++it) {
        PDFJob *j = *it;
        if (j->type() == PDFJob::RenderPageJob) {
            RenderPageJob *rj = static_cast<RenderPageJob *>(j);
            if (rj->m_index == index && rj->renderWidth() == size) {
                if (it != d->thread->jobQueue->begin()) { // If it is already at the front, just abort..
                    d->thread->jobQueue->erase(it);
                    d->thread->jobQueue->push_front(j);
                }
                return;
            }
        }
    }
}

void PDFRenderThreadQueue::processPendingJob()
{
    Thread *t = qobject_cast<Thread *>(QThread::currentThread());

    QMutexLocker locker{ &t->mutex };
    if (!t->jobQueue || count() == 0)
        return;

    PDFJob* job = dequeue();
    switch(job->type()) {
    case PDFJob::LoadDocumentJob:
        d->loadFailure = false;
        break;
    default:
        job->m_document = d->document;
        break;
    }
    locker.unlock();

    job->run();

    locker.relock();

    if (!qobject_cast<Thread *>(QThread::currentThread())->jobQueue) {
        delete job;
       return;
    }

    switch(job->type()) {
        case PDFJob::LoadDocumentJob: {
            LoadDocumentJob* dj = static_cast< LoadDocumentJob* >( job );
            if( d->document )
                delete d->document;
            if(d->tocModel) {
                d->tocModel->deleteLater();
                d->tocModel = nullptr;
            }
    
            d->document = dj->m_document;
            if(d->document) {
                if (!d->document->isLocked()) {
                    d->tocModel = new PDFTocModel(d->document);
                    d->rescanDocumentLinks();
                }
            } else {
                d->loadFailure = true;
            }
            job->deleteLater();
            emit d->q->loadFinished();
            break;
        }
        default: {
            emit d->q->jobFinished(job);
            break;
        }
    }
}

bool PDFRenderThreadQueue::event(QEvent *e)
{
    if (e->type() == Event_JobPending) {
        processPendingJob();
        return true;
    }
    return QObject::event(e);
}

#include "pdfrenderthread.moc"
