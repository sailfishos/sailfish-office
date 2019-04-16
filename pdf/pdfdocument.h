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

#include <poppler-qt5.h>

class PDFJob;

class PDFDocument : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString autoSavePath READ autoSavePath WRITE setAutoSavePath NOTIFY autoSavePathChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)
    Q_PROPERTY(QObject* tocModel READ tocModel NOTIFY tocModelChanged)
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY documentLoadedChanged)
    Q_PROPERTY(bool failure READ isFailed NOTIFY documentFailedChanged)
    Q_PROPERTY(bool locked READ isLocked NOTIFY documentLockedChanged)
    Q_PROPERTY(bool modified READ isModified NOTIFY documentModifiedChanged)
    Q_PROPERTY(bool searching READ searching NOTIFY searchingChanged)
    Q_PROPERTY(QObject* searchModel READ searchModel NOTIFY searchModelChanged)

    Q_INTERFACES(QQmlParserStatus)

public:
    PDFDocument(QObject *parent = 0);
    ~PDFDocument();

public:
    typedef QList<QPair<QRectF, QUrl> > LinkList;
    typedef QList<QPair<QRectF, Poppler::TextBox*> > TextList;

    QString source() const;
    QString autoSavePath() const;
    int pageCount() const;
    QObject* tocModel() const;
    bool searching() const;
    QObject* searchModel() const;
    
    TextList textBoxesAtPage(int page);

    bool isLoaded() const;
    bool isFailed() const;
    bool isLocked() const;
    bool isModified() const;

    void addAnnotation(Poppler::Annotation *annotation, int pageIndex,
                       bool normalizeSize = false);
    QList<Poppler::Annotation*> annotations(int page) const;
    void removeAnnotation(Poppler::Annotation *annotation, int pageIndex);

    void setDocumentModified();

    virtual void classBegin();
    virtual void componentComplete();

public Q_SLOTS:
    void setSource(const QString &source);
    void setAutoSavePath(const QString &filename);
    void requestUnLock(const QString &password);
    void requestLinksAtPage(int page);
    void requestPage(int index, int size, QQuickWindow *window, QRect subpart = QRect(),
                     Poppler::Page::Rotation rotation = Poppler::Page::Rotate0,
                     int extraData = 0);
    void prioritizeRequest(int index, int size, QRect subpart = QRect());
    void cancelPageRequest(int index);
    void requestPageSizes();
    void search(const QString &search, uint startPage = 0);
    void cancelSearch(bool resetModel = true);
    void onSearchFinished();
    void onSearchProgress(float fraction, const QList<QPair<int, QRectF>> &matches);
    void loadFinished();
    void jobFinished(PDFJob *job);
    void onPageModified(int page, const QRectF &subpart);

Q_SIGNALS:
    void sourceChanged();
    void autoSavePathChanged();
    void pageCountChanged();
    void tocModelChanged();
    void searchingChanged();
    void searchModelChanged();
    void pageModified(int index, const QRectF &subpart);

    void documentLoadedChanged();
    void documentFailedChanged();
    void documentLockedChanged();
    void documentModifiedChanged();
    void linksFinished(int page, const LinkList &links);
    void pageFinished(int index, int resolution, QRect subpart,
                      QSGTexture *page, int extraData);
    void pageSizesFinished(const QList<QSizeF> &heights);

private:
    class Private;
    Private * const d;
};

#endif // PDFDOCUMENT_H
