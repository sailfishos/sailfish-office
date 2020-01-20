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

#ifndef PDFJOB_H
#define PDFJOB_H

#include <QString>
#include <QImage>
#include <QObject>
#include <QtQuick/QQuickWindow>

namespace Poppler
{
    class Document;
}

class PDFJob : public QObject
{
    Q_OBJECT
public:
    enum JobType {
        LoadDocumentJob,
        UnLockDocumentJob,
        LinksJob,
        RenderPageJob,
        PageSizesJob,
        SearchDocumentJob,
    };

    PDFJob(JobType type) : m_document(nullptr), m_type(type) { }
    virtual ~PDFJob() { }

    virtual void run() = 0;

    JobType type() const { return m_type; }

protected:
    friend class PDFRenderThreadQueue;
    Poppler::Document *m_document;

private:
    JobType m_type;
};

class LoadDocumentJob : public PDFJob
{
    Q_OBJECT
public:
    LoadDocumentJob(const QString &source);

    virtual void run();

private:
    QString m_source;
};

class UnLockDocumentJob : public PDFJob
{
    Q_OBJECT
public:
    UnLockDocumentJob(const QString &password);

    virtual void run();

private:
    QString m_password;
};

class LinksJob : public PDFJob
{
    Q_OBJECT
public:
    LinksJob(int page);

    virtual void run();

    int m_page;
    QList<QPair<QRectF, QUrl> > m_links;
};

class RenderPageJob : public PDFJob
{
    Q_OBJECT
public:
    RenderPageJob(int requestId, int index, uint width,
                  QRect  subpart = QRect(), int extraData = 0);

    virtual void run();

    int m_requestId;
    int m_index;
    QRect m_subpart;
    QImage m_page;
    int m_extraData;

    int renderWidth() const { return m_width; }
    void changeRenderWidth(int width) { m_width = width; }

private:
    uint m_width;
};

class PageSizesJob : public PDFJob
{
    Q_OBJECT
public:
    PageSizesJob() : PDFJob(PDFJob::PageSizesJob) { }

    virtual void run();

    QList<QSizeF> m_pageSizes;
};


#endif // PDFJOB_H
