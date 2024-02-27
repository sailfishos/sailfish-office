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

#include "pdfdocument.h"
#include "pdfrenderthread.h"
#include "pdfjob.h"
#include "pdfsearchmodel.h"

#include <QDebug>
#include <QUrl>

#include <poppler-qt5.h>

class PDFDocument::Private
{
public:
    Private()
        : searching(false)
        , searchModel(nullptr)
        , completed(false)
        , modified(false)
    {
    }

    PDFRenderThread *thread;

    bool searching;
    PDFSearchModel *searchModel;

    QString source;
    QString autoSavePath;
    bool completed;
    bool modified;
};

PDFDocument::PDFDocument(QObject *parent)
    : QObject(parent), d(new Private)
{
    d->thread = new PDFRenderThread(this);
    connect(d->thread, &PDFRenderThread::loadFinished, this, &PDFDocument::documentLoadedChanged);
    connect(d->thread, &PDFRenderThread::loadFinished, this, &PDFDocument::pageCountChanged);
    connect(d->thread, &PDFRenderThread::loadFinished, this, &PDFDocument::loadFinished);
    connect(d->thread, &PDFRenderThread::loadFinished, this, &PDFDocument::passwordChanged);
    connect(d->thread, &PDFRenderThread::jobFinished, this, &PDFDocument::jobFinished);
    connect(d->thread, &PDFRenderThread::searchFinished, this, &PDFDocument::onSearchFinished);
    connect(d->thread, &PDFRenderThread::searchProgress, this, &PDFDocument::onSearchProgress);
    connect(d->thread, &PDFRenderThread::pageModified, this, &PDFDocument::onPageModified);
}

PDFDocument::~PDFDocument()
{
    delete d->thread;
    delete d->searchModel;
    delete d;
}

QString PDFDocument::source() const
{
    return d->source;
}

QString PDFDocument::autoSavePath() const
{
    return d->autoSavePath;
}

int PDFDocument::pageCount() const
{
    if (d->thread && d->thread->isLoaded()) {
        return d->thread->pageCount();
    }

    return 0;
}

QObject* PDFDocument::tocModel() const
{
    return d->thread->tocModel();
}

QObject* PDFDocument::searchModel() const
{
    return d->searchModel;
}

bool PDFDocument::searching() const
{
    return d->searching;
}

bool PDFDocument::isLoaded() const
{
    return d->thread->isLoaded();
}

bool PDFDocument::isPasswordProtected() const
{
    return d->thread->isPasswordProtected();
}

bool PDFDocument::isFailed() const
{
    return d->thread->isFailed();
}

bool PDFDocument::isLocked() const
{
    return d->thread->isLocked();
}

bool PDFDocument::isModified() const
{
    return d->modified;
}

QString PDFDocument::password() const
{
    return d->thread->cachedPassword();
}

void PDFDocument::clearCachedPassword() const
{
    ClearSecretJob* job = new ClearSecretJob(d->source);
    d->thread->queueJob(job);
}

PDFDocument::TextList PDFDocument::textBoxesAtPage(int page)
{
    return d->thread->textBoxesAtPage(page);
}

void PDFDocument::classBegin()
{
}

void PDFDocument::componentComplete()
{
    if (!d->source.isEmpty()) {
        LoadDocumentJob* job = new LoadDocumentJob(QUrl(d->source).toLocalFile());
        d->thread->queueJob(job);
    }

    d->completed = true;
}

void PDFDocument::setSource(const QString &source)
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

void PDFDocument::setAutoSavePath(const QString &filename)
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

void PDFDocument::addAnnotation(Poppler::Annotation *annotation, int pageIndex,
                                bool normalizeSize)
{
    if (pageIndex < 0) {
        qWarning() << "Invalid page index for annotation";
    } else {
        d->thread->addAnnotation(annotation, pageIndex, normalizeSize);
    }
}

QList<Poppler::Annotation*> PDFDocument::annotations(int page) const
{
    return d->thread->annotations(page);
}

void PDFDocument::removeAnnotation(Poppler::Annotation *annotation, int pageIndex)
{
    d->thread->removeAnnotation(annotation, pageIndex);
}

void PDFDocument::setDocumentModified()
{
    if (d->modified)
        return;

    d->modified = true;
    if (!d->autoSavePath.isEmpty())
        d->thread->setAutoSaveName(QUrl(d->autoSavePath).toLocalFile());
}

void PDFDocument::requestUnLock(const QString &password, bool store)
{
    if (!isLocked())
        return;

    UnLockDocumentJob* job = new UnLockDocumentJob(password, (store) ? d->source : QString());
    d->thread->queueJob(job);
}

void PDFDocument::requestLinksAtPage(int page)
{
    if (!isLoaded() || isLocked())
        return;

    LinksJob* job = new LinksJob(page);
    d->thread->queueJob(job);
}

int PDFDocument::requestPage(int index, int size,
                              QRect subpart, int extraData)
{
    if (!isLoaded() || isLocked())
        return 0;

    static int requestIdCounter = 0;

    int requestId = ++requestIdCounter;

    RenderPageJob* job = new RenderPageJob(requestId, index, size, subpart, extraData);
    d->thread->queueJob(job);

    return requestId;
}

void PDFDocument::prioritizeRequest(int index, int size, QRect subpart)
{
    if (!isLoaded() || isLocked())
        return;
    d->thread->prioritizeRenderJob(index, size, subpart);
}

bool PDFDocument::cancelPageRequest(int requestId)
{
    if (!isLoaded() || isLocked())
        return false;
    return d->thread->cancelRenderJob(requestId);
}

void PDFDocument::requestPageSizes()
{
    if (!isLoaded() || isLocked())
        return;

    PageSizesJob* job = new PageSizesJob;
    d->thread->queueJob(job);
}

void PDFDocument::search(const QString &search, uint startPage)
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
        d->searchModel = new PDFSearchModel;
        emit searchModelChanged();

        d->searching = true;
        emit searchingChanged();
        d->thread->search(search, startPage);
    } else {
        cancelSearch();
    }
}

void PDFDocument::cancelSearch(bool resetModel)
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

void PDFDocument::loadFinished()
{
    if (d->thread->isFailed())
        emit documentFailedChanged();
    if (d->thread->isLocked())
        emit documentLockedChanged();
}

void PDFDocument::onPageModified(int page, const QRectF &subpart)
{
    setDocumentModified();
    emit pageModified(page, subpart);
}

void PDFDocument::jobFinished(PDFJob *job)
{
    switch(job->type()) {
    case PDFJob::UnLockDocumentJob: {
        emit documentLockedChanged();
        emit pageCountChanged();
        emit passwordChanged();
        break;
    }
    case PDFJob::LinksJob: {
        LinksJob* j = static_cast<LinksJob*>(job);
        emit linksFinished(j->m_page, j->m_links);
        break;
    }
    case PDFJob::RenderPageJob: {
        RenderPageJob* j = static_cast<RenderPageJob*>(job);
        emit pageFinished(j->m_requestId, j->renderWidth(), j->m_subpart,
                          j->m_page, j->m_extraData);
        break;
    }
    case PDFJob::PageSizesJob: {
        PageSizesJob* j = static_cast<PageSizesJob*>(job);
        emit pageSizesFinished(j->m_pageSizes);
        break;
    }
    case PDFJob::ClearSecretJob: {
        emit passwordChanged();
        break;
    }
    default:
        break;
    }

    job->deleteLater();
}

void PDFDocument::onSearchFinished()
{
    d->searching = false;
    emit searchingChanged();
}

void PDFDocument::onSearchProgress(float fraction, const QList<QPair<int, QRectF>> &matches)
{
    if (!d->searchModel)
        return;

    d->searchModel->addMatches(fraction, matches);
}
