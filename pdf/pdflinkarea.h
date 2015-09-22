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

class PDFCanvas;
class PDFLinkArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY( PDFCanvas* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged )

public:
    PDFLinkArea(QQuickItem* parent = 0);
    virtual ~PDFLinkArea();

    PDFCanvas* canvas() const;
    void setCanvas( PDFCanvas* newCanvas );

Q_SIGNALS:
    void clicked();
    void doubleClicked();
    void linkClicked(QUrl linkTarget);
    void gotoClicked(int page, qreal top, qreal left);

    void canvasChanged();

protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

private:
    class Private;
    Private* d;
};

#endif // LINKLAYER_H
