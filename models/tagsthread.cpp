/*
 * Copyright (C) 2015 Damien Caliste
 * Contact: Damien Caliste <dcaliste@free.fr>
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

#include "tagsthread.h"

#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QDebug>
#include <QCoreApplication>

class TagsThreadQueue;

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
        deleteLater();
    }

    bool event(QEvent *e) {
        // intercept deleteLater to wait for this thread to be
        // finished. Called on the GUI thread.
        if (e->type() == QEvent::DeferredDelete)
            wait();
        return QThread::event(e);
    }

    TagsThreadQueue *jobQueue;
    QMutex mutex;
};

class TagsThreadPrivate
{
public:
    TagsThreadPrivate() { }

    Thread *thread;
};

class TagsThreadQueue : public QObject, public QQueue< TagsThreadJob* >
{
public:
    TagsThread *tagsThread;
protected:
    bool event(QEvent *);
    void processPendingJob();
};

TagsThread::TagsThread(QObject* parent)
    : QObject( parent ), priv( new TagsThreadPrivate() )
{
    priv->thread = new Thread();
    priv->thread->jobQueue = new TagsThreadQueue();
    priv->thread->jobQueue->tagsThread = this;
    priv->thread->start();
    priv->thread->jobQueue->moveToThread(priv->thread);
}

TagsThread::~TagsThread()
{
    // Cancel outstanding render jobs and schedule the queue
    // for deletion. Also set the jobQueue to 0 so we don't
    // end up calling back to the now deleted documents object.
    cancelJob(0);
    priv->thread->mutex.lock();
    priv->thread->jobQueue->deleteLater();
    priv->thread->jobQueue = 0;
    priv->thread->mutex.unlock();
    priv->thread->exit();

    delete priv;
}

void TagsThread::queueJob(TagsThreadJob *job)
{
    QMutexLocker locker{ &priv->thread->mutex };
    job->moveToThread( priv->thread );
    priv->thread->jobQueue->enqueue( job );
    QCoreApplication::postEvent(priv->thread->jobQueue, new QEvent(Event_JobPending));
}

void TagsThread::cancelJob(TagsThreadJob *job)
{
    QMutexLocker locker{ &priv->thread->mutex };
    for (QList<TagsThreadJob *>::iterator it = priv->thread->jobQueue->begin(); it != priv->thread->jobQueue->end(); ) {
        TagsThreadJob *j = *it;
        if (!job || j == job) {
            it = priv->thread->jobQueue->erase(it);
            j->deleteLater();
            if (job) {
                continue; // to skip the ++it at the end of the loop
            } else {
                return;
            }
        }
        ++it;
    }
}

void TagsThreadQueue::processPendingJob()
{
    Thread *t = qobject_cast<Thread *>(QThread::currentThread());

    QMutexLocker locker{ &t->mutex };
    if (!t->jobQueue || count() == 0)
        return;

    TagsThreadJob* job = dequeue();
    locker.unlock();
    
    // Retrieve tags here
    job->run();

    locker.relock();

    if (!qobject_cast<Thread *>(QThread::currentThread())->jobQueue) {
        delete job;
        return;
    }

    emit tagsThread->jobFinished(job);
}

bool TagsThreadQueue::event(QEvent *e)
{
    if (e->type() == Event_JobPending) {
        processPendingJob();
        return true;
    }
    return QObject::event(e);
}

TagsThreadJob::~TagsThreadJob()
{
}

void TagsThreadJob::run()
{
    /*if (!strstr(path.toLocal8Bit().data(), ".pdf")) {
        tags.append(QString("PDF"));
    }
    tags.append(QString("newt"));*/
}



#include "tagsthread.moc"
