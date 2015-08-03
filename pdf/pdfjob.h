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
        RenderPageJob,
        PageSizesJob,
        SearchDocumentJob,
    };

    PDFJob( JobType type ) : m_document{ nullptr }, m_type{ type } { }
    virtual ~PDFJob() { }

    virtual void run() = 0;

    JobType type() const { return m_type; }

protected:
    friend class PDFRenderThreadQueue;
    Poppler::Document* m_document;

private:
    JobType m_type;
};

class LoadDocumentJob : public PDFJob
{
    Q_OBJECT
public:
    LoadDocumentJob( const QString& source );

    virtual void run();

private:
    QString m_source;
};

class UnLockDocumentJob : public PDFJob
{
    Q_OBJECT
public:
    UnLockDocumentJob( const QString& password );

    virtual void run();

private:
    QString m_password;
};

class RenderPageJob : public PDFJob
{
    Q_OBJECT
public:
    RenderPageJob(int index, uint width, QQuickWindow *window);

    virtual void run();

    int m_index;
    QSGTexture *m_page;

    int renderWidth() const { return m_width; }

private:
    QQuickWindow *m_window;
    uint m_width;
};

class PageSizesJob : public PDFJob
{
    Q_OBJECT
public:
    PageSizesJob() : PDFJob{ PDFJob::PageSizesJob } { }

    virtual void run();

    QList< QSizeF > m_pageSizes;
};

class SearchDocumentJob : public PDFJob
{
    Q_OBJECT
public:
    SearchDocumentJob( const QString& search, uint page );

    virtual void run();

    QList<QPair<int, QRectF>> m_matches;

private:
    QString m_search;
    uint startPage;
};


#endif // PDFJOB_H
