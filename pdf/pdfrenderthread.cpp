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

#include <poppler-qt5.h>

#include "pdfjob.h"
#include "pdftocmodel.h"

class PDFRenderThreadQueue;

const QEvent::Type Event_JobPending = QEvent::Type(QEvent::User + 1);

class PDFRenderThreadPrivate
{
public:
    PDFRenderThreadPrivate() : document{ nullptr }, tocModel{ nullptr } { }

    PDFRenderThread *q;

    QThread* thread;
    PDFRenderThreadQueue *jobQueue;
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

    void processPendingJob();
};

class PDFRenderThreadQueue : public QObject, public QQueue< PDFJob* >
{
public:
    PDFRenderThreadPrivate *d;
protected:
    bool event(QEvent *);
};

PDFRenderThread::PDFRenderThread(QObject* parent)
    : QObject( parent ), d( new PDFRenderThreadPrivate() )
{
    d->q = this;
    d->thread = new QThread(this);
    d->jobQueue = new PDFRenderThreadQueue();
    d->jobQueue->d = d;

    d->thread->start();
    d->jobQueue->moveToThread(d->thread);
}

PDFRenderThread::~PDFRenderThread()
{
    d->thread->exit();
    d->thread->wait();

    qDeleteAll(*d->jobQueue);

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
    QMutexLocker{ &d->mutex };
    return d->tocModel;
}

bool PDFRenderThread::isLoaded() const
{
    QMutexLocker{ &d->mutex };
    return d->document != nullptr;
}

QMultiMap< int, QPair< QRectF, QUrl > > PDFRenderThread::linkTargets() const
{
    QMutexLocker{ &d->mutex };
    return d->linkTargets;
}

void PDFRenderThread::queueJob(PDFJob* job)
{
    QMutexLocker locker{ &d->mutex };
    job->moveToThread( d->thread );
    d->jobQueue->enqueue( job );
    QCoreApplication::postEvent(d->jobQueue, new QEvent(Event_JobPending));
}

void PDFRenderThread::cancelRenderJob(int index)
{
    QMutexLocker locker{ &d->mutex };
    for (QList<PDFJob *>::iterator it = d->jobQueue->begin(); it != d->jobQueue->end(); ) {
        PDFJob *j = *it;
        if (j->type() == PDFJob::RenderPageJob
                && (index < 0 || static_cast<RenderPageJob *>(j)->m_index == index)) {
            it = d->jobQueue->erase(it);
            j->deleteLater();
            continue; // to skip the ++it at the end of the loop
        }
        ++it;
    }
}

void PDFRenderThread::prioritizeJob(int index, int size)
{
    QMutexLocker locker{ &d->mutex };
    for (QList<PDFJob *>::iterator it = d->jobQueue->begin(); it != d->jobQueue->end(); ++it) {
        PDFJob *j = *it;
        if (j->type() == PDFJob::RenderPageJob) {
            RenderPageJob *rj = static_cast<RenderPageJob *>(j);
            if (rj->m_index == index && rj->renderWidth() == size) {
                if (it != d->jobQueue->begin()) { // If it is already at the front, just abort..
                    d->jobQueue->erase(it);
                    d->jobQueue->push_front(j);
                }
                return;
            }
        }
    }
}

void PDFRenderThreadPrivate::processPendingJob()
{
    QMutexLocker locker{ &mutex };
    if( jobQueue->count() == 0 )
        return;

    PDFJob* job = jobQueue->dequeue();
    if (job->type() != PDFJob::LoadDocumentJob)
        job->m_document = document;
    locker.unlock();

    job->run();

    locker.relock();

    switch(job->type())
    {
        case PDFJob::LoadDocumentJob: {
            LoadDocumentJob* dj = static_cast< LoadDocumentJob* >( job );
            if( document )
                delete document;

            document = dj->m_document;
            if(tocModel) {
                tocModel->deleteLater();
                tocModel = nullptr;
            }
            tocModel = new PDFTocModel{ document };
            rescanDocumentLinks();
            job->deleteLater();
            emit q->loadFinished();
            break;
        }
        default:
            emit q->jobFinished(job);
    }
}

bool PDFRenderThreadQueue::event(QEvent *e)
{
    if (e->type() == Event_JobPending) {
        d->processPendingJob();
        return true;
    }
    return QObject::event(e);
}
