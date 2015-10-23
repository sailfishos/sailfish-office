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

#ifndef PDFRENDERTHREAD_H
#define PDFRENDERTHREAD_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QMultiMap>
#include <QtGui/QImage>

class QSize;
class PDFJob;
class PDFRenderThreadPrivate;

class PDFRenderThread : public QObject
{
    Q_OBJECT
public:
    PDFRenderThread( QObject *parent = 0 );
    ~PDFRenderThread();

    int pageCount() const;
    QObject* tocModel() const;
    bool isLoaded() const;
    bool isFailed() const;
    bool isLocked() const;
    QMultiMap< int, QPair< QRectF, QUrl > > linkTargets() const;

    void queueJob(PDFJob *job);
    void cancelRenderJob(int index);
    void prioritizeJob(int index, int size);

Q_SIGNALS:
    void loadFinished();
    void jobFinished(PDFJob *job);

private:
    friend class PDFRenderThreadPrivate;

    PDFRenderThreadPrivate * const d;
};

#endif // PDFRENDERTHREAD_H
