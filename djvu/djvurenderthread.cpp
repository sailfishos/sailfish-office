/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Copyright (C) 2022 Yura Beznos <yura.beznos@you-ra.info>
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

#include "djvurenderthread.h"
//#include <libdjvu/miniexp.h>
//#include <libdjvu/ddjvuapi.h>

#include <QThread>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QDebug>
#include <QCoreApplication>
#include <QSaveFile>

#include "djvu.h"
#include "djvujob.h"
#include "djvutocmodel.h"

class DJVURenderThreadQueue;

const QEvent::Type Event_JobPending = QEvent::Type(QEvent::User + 1);
/*
class SearchThread: public QThread
{
    Q_OBJECT
public:
    SearchThread(Poppler::Document *document, QObject *parent = 0)
        : QThread(parent), m_document(document)
    {
    }
    ~SearchThread()
    {
        requestInterruption();
        wait();
    }

    QList<QPair<int, QRectF>> m_matches;

    void start(const QString& search, uint startPage = 0)
    {
        requestInterruption();
        wait();

        m_search = search;
        m_startPage = startPage;
        QThread::start();
    }

    void run() {
        m_matches.clear();
        m_prevSearchSize = 0;
        for (int i = 0; i < m_document->numPages() && !isInterruptionRequested(); ++i) {
            int ipage = (m_startPage + i) % m_document->numPages();
            Poppler::Page *page = m_document->page(ipage);

            double sLeft, sTop, sRight, sBottom;
            float scaleW = 1.f / page->pageSizeF().width();
            float scaleH = 1.f / page->pageSizeF().height();
            bool found;
            found = page->search(m_search, sLeft, sTop, sRight, sBottom,
                                 Poppler::Page::FromTop,
                                 Poppler::Page::IgnoreCase);
            while (found) {
                QRectF result;
                result.setLeft(sLeft * scaleW);
                result.setTop(sTop * scaleH);
                result.setRight(sRight * scaleW);
                result.setBottom(sBottom * scaleH);
                m_matches.append(QPair<int, QRectF>(ipage, result));
                found = page->search(m_search, sLeft, sTop, sRight, sBottom,
                                     Poppler::Page::NextResult,
                                     Poppler::Page::IgnoreCase);
            }

            delete page;

            if ((i + 1) % 3 == 0 || i + 1 == m_document->numPages()) {
                emit searchProgress((m_document->numPages() > 1)
                                    ? float(i) / float(m_document->numPages() - 1)
                                    : 1.f,
                                    m_prevSearchSize, m_matches.size() - m_prevSearchSize);
                m_prevSearchSize = m_matches.size();
            }
        }
        if (!isInterruptionRequested())
            emit searchFinished();
    }

signals:
    void searchFinished();
    void searchProgress(float fraction, uint beginIndex, uint nNewMatches);

private:
    Poppler::Document *m_document;
    QString m_search;
    uint m_startPage, m_prevSearchSize;
};
*/
class Thread : public QThread
{
    Q_OBJECT
public:
    Thread()
        : jobQueue(0)
    {
    }

    void run() {
        QThread::exec();
        // Delete pending search and toc that may use document.
        //delete searchThread;
        delete tocModel;
        autoSaveTo();
        //delete document;
        deleteLater();
    }

    bool event(QEvent *e) {
        // intercept deleteLater to wait for this thread to be
        // finished. CAlled on the GUI thread.
        if (e->type() == QEvent::DeferredDelete)
            wait();
        return QThread::event(e);
    }

    DJVURenderThreadQueue *jobQueue;
    QMutex mutex;

    // Used for cleanup only
    QString autoSaveFilename;
    KDjVu *document;
    DJVUTocModel *tocModel;
    //SearchThread *searchThread;

private:
    void autoSaveTo();
};

class DJVURenderThreadPrivate
{
public:
    DJVURenderThreadPrivate()
        : /*searchThread(nullptr),*/ document(nullptr), tocModel(nullptr) { }
    ~DJVURenderThreadPrivate()
    { /*
        for (QMap<int, QList<QPair<QRectF, Poppler::TextBox*> > >::iterator i =
                 textBoxes.begin();
             i != textBoxes.end(); i++) {
            for (QList<QPair<QRectF, Poppler::TextBox*> >::iterator j = i.value().begin();
                 j != i.value().end(); j++) {
                delete j->second;
            }
        }
        for (QMap<int, QList<Poppler::Annotation*> >::iterator i = annotations.begin();
             i != annotations.end(); i++) {
            qDeleteAll(i.value());
        } */
    }


    DJVURenderThread *q;

    Thread *thread;
    //SearchThread *searchThread;

    bool loadFailure;
    KDjVu *document;
    DJVUTocModel *tocModel;

    //QMultiMap<int, QPair<QRectF, QUrl> > linkTargets;
    //QMap<int, QList<QPair <QRectF, Poppler::TextBox*> > > textBoxes;
    //QMap<int, QList<Poppler::Annotation*> > annotations;
/*
    void retrieveTextBoxes(int i)
    {
        if (i < 0 || i >= document->numPages()) {
            return;
        }
        Poppler::Page* page = document->page(i);
        QList<Poppler::TextBox*> plst = page->textList();
        QList<QPair<QRectF, Poppler::TextBox*> > lst;
        for (QList<Poppler::TextBox*>::const_iterator box = plst.begin();
             box != plst.end(); box++) {
            QSizeF psize = page->pageSize();
            QRectF bbox = (*box)->boundingBox();
            QRectF rect{ bbox.x() / psize.width(),
                    bbox.y() / psize.height(),
                    bbox.width() / psize.width(),
                    bbox.height() / psize.height()};
            lst.append(QPair<QRectF, Poppler::TextBox*>{rect, *box});
        }
        textBoxes.insert(i, lst);
        delete page;
    }
    void retrieveAnnotations(int i) {
        if (i < 0 || i >= document->numPages()) {
            return;
        }
        if (annotations.contains(i))
            qDeleteAll(annotations.take(i));
        Poppler::Page* page = document->page(i);
        QList<Poppler::Annotation*> annotationsAt = page->annotations();
        // Add all revisions of every annotation we just retrieved.
        int nFirstLevel = annotationsAt.size();
        for (int j = 0; j < nFirstLevel; j++)
            annotationsAt += annotationsAt[j]->revisions();
        annotations.insert(i, annotationsAt);
        delete(page);
    }*/
};

class DJVURenderThreadQueue : public QObject, public QQueue<DJVUJob*>
{
public:
    DJVURenderThreadPrivate *d;
protected:
    bool event(QEvent *);
    void processPendingJob();
};

DJVURenderThread::DJVURenderThread(QObject *parent)
    : QObject(parent), d(new DJVURenderThreadPrivate)
{
    d->q = this;
    d->loadFailure = false;
    d->thread = new Thread;
    d->thread->jobQueue = new DJVURenderThreadQueue;
    d->thread->jobQueue->d = d;
    d->thread->start();
    d->thread->jobQueue->moveToThread(d->thread);
}

DJVURenderThread::~DJVURenderThread()
{
    // Cancel outstanding render jobs and schedule the queue
    // for deletion. Also set the jobQueue to 0 so we don't
    // end up calling back to the now deleted documents object.
    cancelRenderJob(-1);
    d->thread->mutex.lock();
    d->thread->document = d->document;
    d->thread->tocModel = d->tocModel;
    //d->thread->searchThread = d->searchThread;
    d->thread->jobQueue->deleteLater();
    d->thread->jobQueue = 0;
    d->thread->mutex.unlock();
    d->thread->exit();

    delete d;
}

int DJVURenderThread::pageCount() const
{
    QMutexLocker locker(&d->thread->mutex);

    if (d->document != nullptr /*&&
        !d->document->isLocked()*/) {
        return d->document->pages().count();
    } else {
        return 0;
    }
}

QObject* DJVURenderThread::tocModel() const
{
    QMutexLocker(&d->thread->mutex);
    if (d->document && /*!d->document->isLocked() &&*/ !d->tocModel)
        d->tocModel = new DJVUTocModel(d->document);
    return d->tocModel;
}

bool DJVURenderThread::isLoaded() const
{
    QMutexLocker(&d->thread->mutex);
    return d->document != nullptr;
}

bool DJVURenderThread::isFailed() const
{
    QMutexLocker(&d->thread->mutex);
    return d->loadFailure;
}
/*
bool DJVURenderThread::isLocked() const
{
    QMutexLocker(&d->thread->mutex);
    return (d->document != nullptr) ? d->document->isLocked() : false;
}
*/
/*
QMultiMap<int, QPair<QRectF, QUrl> > DJVURenderThread::linkTargets() const
{
    QMutexLocker(&d->thread->mutex);
    return d->linkTargets;
}

QList<QPair<QRectF, Poppler::TextBox*> > DJVURenderThread::textBoxesAtPage(int page)
{
    QMutexLocker(&d->thread->mutex);
    if (!d->textBoxes.contains(page))
        d->retrieveTextBoxes(page);
    return d->textBoxes[page];
}

void DJVURenderThread::addAnnotation(Poppler::Annotation *annotation, int pageIndex,
                                    bool normalizeSize)
{
    QMutexLocker(&d->thread->mutex);
    if (!d->document)
        return;
    Poppler::Page *page = d->document->page(pageIndex);
    if (normalizeSize) {
        QSizeF pSize = page->pageSizeF();
        QRectF bounds = annotation->boundary();
        bounds.setWidth(bounds.width() / pSize.width());
        bounds.setHeight(bounds.height() / pSize.height());
        annotation->setBoundary(bounds);
    }
    page->addAnnotation(annotation);
    delete page;
    // annotation cannot be added to d->annotations of this page
    // since the caller is the owner of the object.
    if (d->annotations.contains(pageIndex))
        d->retrieveAnnotations(pageIndex);
    emit pageModified(pageIndex, annotation->boundary());
}

QList<Poppler::Annotation*> DJVURenderThread::annotations(int pageIndex) const
{
    QMutexLocker(&d->thread->mutex);
    if (!d->document)
        return QList<Poppler::Annotation*>();
    if (!d->annotations.contains(pageIndex))
        d->retrieveAnnotations(pageIndex);
    return d->annotations[pageIndex];
}

void DJVURenderThread::removeAnnotation(Poppler::Annotation *annotation, int pageIndex)
{
    QMutexLocker(&d->thread->mutex);
    if (!d->document)
        return;
    if (d->annotations.contains(pageIndex))
        d->annotations[pageIndex].removeOne(annotation);
    QRectF boundary(annotation->boundary());
    Poppler::Page *page = d->document->page(pageIndex);
    page->removeAnnotation(annotation);
    delete page;
    emit pageModified(pageIndex, boundary);
}
*/
void DJVURenderThread::setAutoSaveName(const QString &filename)
{
    QMutexLocker(&d->thread->mutex);
    d->thread->autoSaveFilename = filename;
}

void DJVURenderThread::queueJob(DJVUJob *job)
{
    QMutexLocker locker(&d->thread->mutex);
    job->moveToThread(d->thread);
    d->thread->jobQueue->enqueue(job);
    QCoreApplication::postEvent(d->thread->jobQueue, new QEvent(Event_JobPending));
}

bool DJVURenderThread::cancelRenderJob(int requestId)
{
    bool deleted = false;
    QMutexLocker locker(&d->thread->mutex);
    for (QList<DJVUJob *>::iterator it = d->thread->jobQueue->begin(); it != d->thread->jobQueue->end(); ) {
        DJVUJob *j = *it;
        if (j->type() == DJVUJob::RenderPageJob
                && (requestId < 0 || static_cast<RenderPageJob *>(j)->m_requestId == requestId)) {
            it = d->thread->jobQueue->erase(it);
            j->deleteLater();
            deleted = true;
        } else {
            ++it;
        }
    }
    return deleted;
}

void DJVURenderThread::prioritizeRenderJob(int index, int size, QRect subpart)
{
    QMutexLocker locker(&d->thread->mutex);
    for (QList<DJVUJob *>::iterator it = d->thread->jobQueue->begin(); it != d->thread->jobQueue->end(); ++it) {
        DJVUJob *j = *it;
        if (j->type() == DJVUJob::RenderPageJob &&
            static_cast<RenderPageJob *>(j)->m_index == index) {
            RenderPageJob *rj = static_cast<RenderPageJob *>(j);
            // Update if necessary before prioritize.
            rj->changeRenderWidth(size);
            rj->m_subpart = subpart;
            if (it != d->thread->jobQueue->begin()) { // If it is already at the front, just abort..
                d->thread->jobQueue->erase(it);
                d->thread->jobQueue->push_front(j);
            }
            return;
        }
    }
}
/*
void DJVURenderThread::search(const QString &search, uint startPage)
{
    if (!d->searchThread) {
        d->searchThread = new SearchThread(d->document);
        connect(d->searchThread, &SearchThread::searchFinished,
                this, &DJVURenderThread::searchFinished);
        connect(d->searchThread, &SearchThread::searchProgress,
                this, &DJVURenderThread::onSearchProgress);
    }

    d->searchThread->start(search, startPage);
}

void DJVURenderThread::onSearchProgress(float fraction, uint indexBegin, uint nNewMatches)
{
    emit searchProgress(fraction, d->searchThread->m_matches.mid(indexBegin, nNewMatches));
}

void DJVURenderThread::cancelSearch()
{
    if (!d->searchThread)
        return;

    d->searchThread->requestInterruption();
}
*/
void DJVURenderThreadQueue::processPendingJob()
{
    Thread *t = qobject_cast<Thread *>(QThread::currentThread());

    QMutexLocker locker(&t->mutex);
    if (!t->jobQueue || count() == 0)
        return;

    DJVUJob *job = dequeue();
    switch(job->type()) {
    case DJVUJob::LoadDocumentJob:
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
        case DJVUJob::LoadDocumentJob: {
            LoadDocumentJob *dj = static_cast<LoadDocumentJob*>(job);
            delete d->document;

            if (d->tocModel) {
                d->tocModel->deleteLater();
                d->tocModel = nullptr;
            }
    
            d->document = dj->m_document;

            if (!d->document) { //|| (!d->document->isLocked() && d->document->numPages() == 0)) {
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

bool DJVURenderThreadQueue::event(QEvent *e)
{
    if (e->type() == Event_JobPending) {
        processPendingJob();
        return true;
    }
    return QObject::event(e);
}

void Thread::autoSaveTo()
{
    QMutexLocker locker(&mutex);

    if (autoSaveFilename.isEmpty())
        return;

    Q_ASSERT(document);
/*
    QSaveFile destination(autoSaveFilename);

    Poppler::DJVUConverter *converter = document->DJVUConverter();
    converter->setOutputDevice(&destination);
    converter->setDJVUOptions(Poppler::DJVUConverter::DJVUOption::WithChanges);
    bool success = converter->convert();
    delete converter;

    if (success)
        destination.commit();
    else
        qWarning() << QStringLiteral("DJVU exportation failure to '%1' (error code %2)").arg(autoSaveFilename).arg(int(converter->lastError()));
        */
}

#include "djvurenderthread.moc"
