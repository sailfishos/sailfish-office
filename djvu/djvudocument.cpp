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

#include "djvudocument.h"

#include "djvu.h"
#include "djvurenderthread.h"
#include "djvujob.h"
/*
#include "DJVUsearchmodel.h"
*/
#include <QDebug>
#include <QUrl>

class DJVUDocument::Private
{
public:
    Private()
        //: searching(false)
        //, searchModel(nullptr)
        : completed(false)
        //, modified(false)
    {
    }

    DJVURenderThread *thread;

    bool searching;
    //DJVUSearchModel *searchModel;

    QString source;
    QString autoSavePath;
    bool completed;
    bool modified;
};

DJVUDocument::DJVUDocument(QObject *parent)
    : QObject(parent), d(new Private)
{
    d->thread = new DJVURenderThread(this);
    connect(d->thread, &DJVURenderThread::loadFinished, this, &DJVUDocument::documentLoadedChanged);

    connect(d->thread, &DJVURenderThread::loadFinished, this, &DJVUDocument::pageCountChanged);
    connect(d->thread, &DJVURenderThread::loadFinished, this, &DJVUDocument::loadFinished);

    connect(d->thread, &DJVURenderThread::jobFinished, this, &DJVUDocument::jobFinished);
    /*
    connect(d->thread, &DJVURenderThread::searchFinished, this, &DJVUDocument::onSearchFinished);
    connect(d->thread, &DJVURenderThread::searchProgress, this, &DJVUDocument::onSearchProgress);
    */
    connect(d->thread, &DJVURenderThread::pageModified, this, &DJVUDocument::onPageModified);

}

DJVUDocument::~DJVUDocument()
{

    delete d->thread;
    //delete d->searchModel;
    delete d;

}

QString DJVUDocument::source() const
{
    return d->source;
}

QString DJVUDocument::autoSavePath() const
{
    return d->autoSavePath;
}

int DJVUDocument::pageCount() const
{
    if (d->thread && d->thread->isLoaded()) {
        return d->thread->pageCount();
    }

    return 0;
}
/*
QObject* DJVUDocument::tocModel() const
{
    return d->thread->tocModel();
}

QObject* DJVUDocument::searchModel() const
{
    return d->searchModel;
}

bool DJVUDocument::searching() const
{
    return d->searching;
}
*/
bool DJVUDocument::isLoaded() const
{
    return d->thread->isLoaded();
}

bool DJVUDocument::isFailed() const
{
    return d->thread->isFailed();
}
/*
bool DJVUDocument::isLocked() const
{
    return d->thread->isLocked();
}

bool DJVUDocument::isModified() const
{
    return d->modified;
}

DJVUDocument::TextList DJVUDocument::textBoxesAtPage(int page)
{
    return d->thread->textBoxesAtPage(page);
}
*/
void DJVUDocument::classBegin()
{
}

void DJVUDocument::componentComplete()
{
    if (!d->source.isEmpty()) {
        LoadDocumentJob* job = new LoadDocumentJob(QUrl(d->source).toLocalFile());
        d->thread->queueJob(job);
    }

    d->completed = true;

}

void DJVUDocument::setSource(const QString &source)
{
    if (d->source != source) {
        d->source = source;
        if (source.startsWith("/"))
            d->source.prepend("file://");

        if (d->completed) {
            LoadDocumentJob* job = new LoadDocumentJob(QUrl(source).toLocalFile());
            d->thread->queueJob(job);
        }

        emit sourceChanged();
    }
}

void DJVUDocument::setAutoSavePath(const QString &filename)
{
    if (d->autoSavePath != filename) {
        d->autoSavePath = filename;
        if (filename.startsWith("/"))
            d->autoSavePath.prepend("file://");

        emit autoSavePathChanged();

        if (d->modified)
            d->thread->setAutoSaveName(QUrl(d->autoSavePath).toLocalFile());
    }
}
/*
void DJVUDocument::addAnnotation(Poppler::Annotation *annotation, int pageIndex,
                                bool normalizeSize)
{
    if (pageIndex < 0) {
        qWarning() << "Invalid page index for annotation";
    } else {
        d->thread->addAnnotation(annotation, pageIndex, normalizeSize);
    }
}

QList<Poppler::Annotation*> DJVUDocument::annotations(int page) const
{
    return d->thread->annotations(page);
}

void DJVUDocument::removeAnnotation(Poppler::Annotation *annotation, int pageIndex)
{
    d->thread->removeAnnotation(annotation, pageIndex);
}
*/
void DJVUDocument::setDocumentModified()
{
    if (d->modified)
        return;

    d->modified = true;
    if (!d->autoSavePath.isEmpty())
        d->thread->setAutoSaveName(QUrl(d->autoSavePath).toLocalFile());
}
/*
void DJVUDocument::requestUnLock(const QString &password)
{
    if (!isLocked())
        return;

    UnLockDocumentJob* job = new UnLockDocumentJob(password);
    d->thread->queueJob(job);
}

void DJVUDocument::requestLinksAtPage(int page)
{
    if (!isLoaded() || isLocked())
        return;

    LinksJob* job = new LinksJob(page);
    d->thread->queueJob(job);
}
*/
int DJVUDocument::requestPage(int index, int size,
                              QRect subpart, int extraData)
{

    if (!isLoaded())// || isLocked())
        return 0;

    static int requestIdCounter = 0;

    int requestId = ++requestIdCounter;

    RenderPageJob* job = new RenderPageJob(requestId, index, size, subpart, extraData);
    d->thread->queueJob(job);

    return requestId;
}

void DJVUDocument::prioritizeRequest(int index, int size, QRect subpart)
{
    if (!isLoaded())// || isLocked())
        return;
    d->thread->prioritizeRenderJob(index, size, subpart);
}

bool DJVUDocument::cancelPageRequest(int requestId)
{
    if (!isLoaded())// || isLocked())
        return false;
    return d->thread->cancelRenderJob(requestId);
}

void DJVUDocument::requestPageSizes()
{
    if (!isLoaded() )//|| isLocked())
        return;

    PageSizesJob* job = new PageSizesJob;
    d->thread->queueJob(job);
}
/*
void DJVUDocument::search(const QString &search, uint startPage)
{
    if (!isLoaded() || isLocked())
        return;

    if (search.length() > 0) {
        delete d->searchModel;
        // Ensure that QML is updated in the repeater displaying
        // the search results, even in case the later 'new' allocates
        // a new object on the same address.
        d->searchModel = nullptr;
        emit searchModelChanged();
        d->searchModel = new DJVUSearchModel;
        emit searchModelChanged();

        d->searching = true;
        emit searchingChanged();
        d->thread->search(search, startPage);
    } else {
        cancelSearch();
    }
}

void DJVUDocument::cancelSearch(bool resetModel)
{
    if (resetModel && d->searchModel != nullptr) {
        delete d->searchModel;
        d->searchModel = nullptr;
        emit searchModelChanged();
    }
    if (d->searching) {
        d->searching = false;
        emit searchingChanged();
    }
    d->thread->cancelSearch();
}
*/
void DJVUDocument::loadFinished()
{
    if (d->thread->isFailed())
        emit documentFailedChanged();
    //if (d->thread->isLocked())
    //    emit documentLockedChanged();
}

void DJVUDocument::onPageModified(int page, const QRectF &subpart)
{
    setDocumentModified();
    emit pageModified(page, subpart);
}

void DJVUDocument::jobFinished(DJVUJob *job)
{
    switch(job->type()) {
//    case DJVUJob::UnLockDocumentJob: {
//        emit documentLockedChanged();
//        emit pageCountChanged();
//        break;
//    }
//    case DJVUJob::LinksJob: {
//        LinksJob* j = static_cast<LinksJob*>(job);
//        emit linksFinished(j->m_page, j->m_links);
//        break;
//    }
    case DJVUJob::RenderPageJob: {
        RenderPageJob* j = static_cast<RenderPageJob*>(job);
        emit pageFinished(j->m_requestId, j->renderWidth(), j->m_subpart,
                          j->m_page, j->m_extraData);
        break;
    }
    case DJVUJob::PageSizesJob: {
        PageSizesJob* j = static_cast<PageSizesJob*>(job);
        emit pageSizesFinished(j->m_pageSizes);
        break;
    }
    default:
        break;
    }

    job->deleteLater();
}
/*
void DJVUDocument::onSearchFinished()
{
    d->searching = false;
    emit searchingChanged();
}

void DJVUDocument::onSearchProgress(float fraction, const QList<QPair<int, QRectF>> &matches)
{
    if (!d->searchModel)
        return;

    d->searchModel->addMatches(fraction, matches);
}
*/
