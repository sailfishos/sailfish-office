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

#ifndef LINKLAYER_H
#define LINKLAYER_H

#include <QtQuick/QQuickPaintedItem>
#include "pdfannotation.h"

class PDFCanvas;
class PDFSelection;

class PDFLinkArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(PDFCanvas* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(QRectF clickedBox READ clickedBox NOTIFY clickedBoxChanged)
    Q_PROPERTY(PDFSelection* selection READ selection WRITE setSelection NOTIFY selectionChanged)

public:
    PDFLinkArea(QQuickItem *parent = 0);
    virtual ~PDFLinkArea();

    PDFCanvas* canvas() const;
    bool pressed() const;
    QRectF clickedBox() const;
    void setCanvas(PDFCanvas *newCanvas);
    PDFSelection* selection() const;
    void setSelection(PDFSelection *newSelection);

Q_SIGNALS:
    void pressedChanged();
    void clickedBoxChanged();
    void clicked(QPointF clickAt);
    void doubleClicked();
    void linkClicked(QUrl linkTarget);
    void gotoClicked(int page, qreal top, qreal left);
    void selectionClicked();
    void annotationClicked(PDFAnnotation *annotation);
    void annotationLongPress(PDFAnnotation *annotation);
    void longPress(QPointF pressAt);

    void canvasChanged();
    void selectionChanged();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseUngrabEvent();

private Q_SLOTS:
    void pressTimeout();
    void onPageLayoutChanged();

private:
    class Private;
    Private *d;

    PDFAnnotation* newProxyForAnnotation();
};

#endif // LINKLAYER_H
