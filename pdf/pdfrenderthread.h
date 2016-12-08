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

#include <poppler-qt5.h>

class QSize;
class PDFJob;
class PDFRenderThreadPrivate;

class PDFRenderThread : public QObject
{
    Q_OBJECT
public:
    PDFRenderThread(QObject *parent = 0);
    ~PDFRenderThread();

    int pageCount() const;
    QObject* tocModel() const;
    bool isLoaded() const;
    bool isFailed() const;
    bool isLocked() const;
    QMultiMap<int, QPair<QRectF, QUrl> > linkTargets() const;
    QList<QPair<QRectF, Poppler::TextBox*> > textBoxesAtPage(int page);
    void search(const QString &search, uint startPage);
    void cancelSearch();

    void addAnnotation(Poppler::Annotation *annotation, int pageIndex,
                       bool normalizeSize);
    QList<Poppler::Annotation*> annotations(int pageIndex) const;
    void removeAnnotation(Poppler::Annotation *annotation, int pageIndex);

    void setAutoSaveName(const QString &filename);

    void queueJob(PDFJob *job);
    void cancelRenderJob(int index);
    void prioritizeRenderJob(int index, int size, QRect subpart);

Q_SIGNALS:
    void loadFinished();
    void pageModified(int page, const QRectF &subpart);
    void jobFinished(PDFJob *job);
    void searchFinished();
    void searchProgress(float fraction, const QList<QPair<int, QRectF>> &newMatches);

private Q_SLOTS:
    void onSearchProgress(float fraction, uint beginIndex, uint nNewMatches);
    
private:
    friend class PDFRenderThreadPrivate;

    PDFRenderThreadPrivate * const d;
};

#endif // PDFRENDERTHREAD_H
