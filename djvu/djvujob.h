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

#ifndef DJVUJOB_H
#define DJVUJOB_H

#include <QString>
#include <QImage>
#include <QObject>
#include <QtQuick/QQuickWindow>
#include "djvu.h"

/*
namespace Poppler
{
    class Document;
}
*/


class DJVUJob : public QObject
{
    Q_OBJECT
public:
    enum JobType {
        LoadDocumentJob,
        //UnLockDocumentJob,
        //LinksJob,
        RenderPageJob,
        PageSizesJob,
        //SearchDocumentJob,
    };

    DJVUJob(JobType type) : m_document(nullptr), m_type(type) { }
    virtual ~DJVUJob() { }

    virtual void run() = 0;

    JobType type() const { return m_type; }

protected:
    friend class DJVURenderThreadQueue;
    //Poppler::Document *m_document;
    KDjVu *m_document;

private:
    JobType m_type;
};

class LoadDocumentJob : public DJVUJob
{
    Q_OBJECT
public:
    LoadDocumentJob(const QString &source);

    virtual void run();

private:
    QString m_source;
};
/*
class UnLockDocumentJob : public DJVUJob
{
    Q_OBJECT
public:
    UnLockDocumentJob(const QString &password);

    virtual void run();

private:
    QString m_password;
};

class LinksJob : public DJVUJob
{
    Q_OBJECT
public:
    LinksJob(int page);

    virtual void run();

    int m_page;
    QList<QPair<QRectF, QUrl> > m_links;
};
*/

class RenderPageJob : public DJVUJob
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
    //QSizeF size;
};

class PageSizesJob : public DJVUJob
{
    Q_OBJECT
public:
    PageSizesJob() : DJVUJob(DJVUJob::PageSizesJob) { }

    virtual void run();

    QList<QSizeF> m_pageSizes;
};


#endif // DJVUJOB_H
