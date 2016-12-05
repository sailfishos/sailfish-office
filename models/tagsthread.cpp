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
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QStandardPaths>
#include <QDir>

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
    cancelAllJobs();
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

void TagsThread::cancelAllJobs()
{
    QMutexLocker locker{ &priv->thread->mutex };
    for (QList<TagsThreadJob *>::iterator it = priv->thread->jobQueue->begin(); it != priv->thread->jobQueue->end(); ) {
        TagsThreadJob *j = *it;
        it = priv->thread->jobQueue->erase(it);
        j->deleteLater();
    }
}
void TagsThread::cancelJobsForPath( const QString &path )
{
    QMutexLocker locker{ &priv->thread->mutex };
    for (QList<TagsThreadJob *>::iterator it = priv->thread->jobQueue->begin(); it != priv->thread->jobQueue->end(); ) {
      if (path == (*it)->path) {
            TagsThreadJob *j = *it;
            it = priv->thread->jobQueue->erase(it);
            j->deleteLater();
        } else {
            ++it;
        }
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
    switch (task) {
    case (TaskLoadTags):
        loadTagsFromDb();
        break;
    case (TaskAddTags):
        addTagsToDb();
        break;
    case (TaskRemoveTags):
        removeTagsFromDb();
        break;
    }
}


/* Part related to access the local storage, as available from QML side. */
static const QString dbConnection{"TagsStorage"};
static void addDatabase()
{
    QString basename = QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
        QDir::separator() + dbConnection + QLatin1String(".sqlite");
    QSqlDatabase database = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"),
                                                      dbConnection);
    database.setDatabaseName(basename);
}

static void ensureDbConnection()
{
    if (!QSqlDatabase::contains(dbConnection)) {
        addDatabase();
    }
}
static bool ensureTable(QSqlDatabase &db)
{
    bool ret;
    QSqlQuery qCreate = QSqlQuery(db);
    ret = qCreate.exec(QLatin1String("CREATE TABLE IF NOT EXISTS Tags("
                                     "file TEXT   NOT NULL,"
                                     "tag  TEXT   NOT NULL)"));
    if (!ret)
        return false;
  
    QSqlQuery qIndex = QSqlQuery(db);
    ret = qIndex.exec(QLatin1String("CREATE INDEX IF NOT EXISTS idx_file ON Tags(file)"));
    return ret;
}

void TagsThreadJob::loadTagsFromDb()
{
    ensureDbConnection();
    QSqlDatabase db = QSqlDatabase::database(dbConnection);
    if (!ensureTable(db))
        return;

    QSqlQuery query = QSqlQuery(db);
    query.prepare(QLatin1String("SELECT tag FROM Tags WHERE file = ?"));
    query.addBindValue(path);

    if (!query.exec())
        return;

    while (query.next()) {
        tags.append(query.value(0).toString());
    }
}
void TagsThreadJob::addTagsToDb()
{
    ensureDbConnection();
    QSqlDatabase db = QSqlDatabase::database(dbConnection);
    if (!ensureTable(db))
        return;
    
    for (QList<QString>::const_iterator tag = tags.begin(); tag != tags.end(); tag++ ) {
        QSqlQuery query = QSqlQuery(db);
        query.prepare(QLatin1String("INSERT INTO Tags(file, tag) VALUES (?, ?)"));
        query.addBindValue(path);
        query.addBindValue(*tag);

        if (!query.exec())
            return;
    }
}
void TagsThreadJob::removeTagsFromDb()
{
    ensureDbConnection();
    QSqlDatabase db = QSqlDatabase::database(dbConnection);
    if (!ensureTable(db))
        return;
    
    for (QList<QString>::const_iterator tag = tags.begin(); tag != tags.end(); tag++ ) {
        QSqlQuery query = QSqlQuery(db);
        query.prepare(QLatin1String("DELETE FROM Tags WHERE file = ? AND tag = ?"));
        query.addBindValue(path);
        query.addBindValue(*tag);

        if (!query.exec())
            return;
    }
}


#include "tagsthread.moc"
