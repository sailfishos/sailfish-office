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

#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include <QtCore/QObject>
#include <QtGui/QImage>
#include <QtQml/QQmlParserStatus>

#include <QtQuick/QSGTexture>
#include <QtQuick/QQuickWindow>

namespace Poppler {
    class Document;
    class Page;
}

class PDFJob;
class PDFDocument : public QObject, public QQmlParserStatus

{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)
    Q_PROPERTY(QObject* tocModel READ tocModel NOTIFY tocModelChanged)
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY documentLoaded)
    Q_PROPERTY(bool failure READ isFailed NOTIFY documentFailed)

    Q_INTERFACES(QQmlParserStatus)

public:
    PDFDocument(QObject* parent = 0);
    ~PDFDocument();

public:
    typedef QMultiMap< int, QPair< QRectF, QUrl > > LinkMap;

    QString source() const;
    int pageCount() const;
    QObject* tocModel() const;

    LinkMap linkTargets() const;

    bool isLoaded() const;
    bool isFailed() const;

    virtual void classBegin();
    virtual void componentComplete();

public Q_SLOTS:
    void setSource(const QString& source);
    void requestPage( int index, int size, QQuickWindow *window );
    void prioritizeRequest( int index, int size);
    void cancelPageRequest(int index);
    void requestPageSizes();
    void jobFinished(PDFJob* job);

Q_SIGNALS:
    void sourceChanged();
    void pageCountChanged();
    void tocModelChanged();

    void documentLoaded();
    void documentFailed();
    void pageFinished( int index, QSGTexture *page );
    void pageSizesFinished(const QList< QSizeF >& heights);

private:
    class Private;
    Private * const d;
};

#endif // PDFDOCUMENT_H
