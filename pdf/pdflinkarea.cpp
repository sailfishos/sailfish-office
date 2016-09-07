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

#include "pdflinkarea.h"
#include "pdfcanvas.h"
#include <QUrlQuery>
#include <QTimer>

class PDFLinkArea::Private
{
public:
    Private()
        : canvas(nullptr)
        , wiggleFactor(4)
    { }

    PDFCanvas *canvas;

    QPointF clickLocation;
    int wiggleFactor;

    QTimer pressTimer;
    bool pressed;
    PDFCanvas::ReducedBox pressedBox;
    QUrl link;
};

PDFLinkArea::PDFLinkArea(QQuickItem *parent)
    : QQuickItem(parent)
    , d(new Private)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);

    d->pressTimer.setInterval(750);
    d->pressTimer.setSingleShot(true);
    connect(&d->pressTimer, &QTimer::timeout, this, &PDFLinkArea::pressTimeout);
}

PDFLinkArea::~PDFLinkArea()
{
    delete d;
}

PDFCanvas* PDFLinkArea::canvas() const
{
    return d->canvas;
}

void PDFLinkArea::setCanvas(PDFCanvas *newCanvas)
{
    if (newCanvas != d->canvas) {
        if (d->canvas)
            d->canvas->disconnect(this);
        
        d->canvas = newCanvas;
        connect(d->canvas, &PDFCanvas::pageLayoutChanged,
                this, &PDFLinkArea::onPageLayoutChanged);

        emit canvasChanged();
    }
}

bool PDFLinkArea::pressed() const
{
    return d->pressed;
}

QRectF PDFLinkArea::clickedBox() const
{
    if (d->canvas && !d->pressedBox.second.isEmpty())
        return d->canvas->fromPageToItem(d->pressedBox.first, d->pressedBox.second);
    else
        return QRectF();
}

void PDFLinkArea::onPageLayoutChanged()
{
    if (d->pressedBox.second.isEmpty())
        return;

    emit clickedBoxChanged();
}

void PDFLinkArea::mousePressEvent(QMouseEvent *event)
{
    // Nullify all handles.
    d->pressedBox.second = QRectF();
    d->link.clear();

    d->clickLocation = event->pos();
    d->pressTimer.start();

    if (!d->canvas)
        return;

    QPair<QUrl, PDFCanvas::ReducedBox> urlAt = d->canvas->urlAtPoint(d->clickLocation);
    d->link = urlAt.first;
    if (!d->link.isEmpty()) {
        d->pressedBox = urlAt.second;
    }
    
    d->pressed = true;
    emit pressedChanged();
    emit clickedBoxChanged();
}

void PDFLinkArea::mouseMoveEvent(QMouseEvent *event)
{
    // Don't activate anything if the finger has moved too far
    QRect rect((d->clickLocation - QPointF(d->wiggleFactor, d->wiggleFactor)).toPoint(),
               QSize(d->wiggleFactor * 2, d->wiggleFactor * 2));
    if (!rect.contains(event->pos())) {
        d->pressTimer.stop();
        d->pressed = false;
        emit pressedChanged();
        return;
    }
}

void PDFLinkArea::mouseReleaseEvent(QMouseEvent *event)
{
    d->pressed = false;
    emit pressedChanged();

    // Don't activate click if the longPress already fired.
    if (!d->pressTimer.isActive())
        return;
    d->pressTimer.stop();

    // Don't activate anything if the finger has moved too far
    QRect rect((d->clickLocation - QPointF(d->wiggleFactor, d->wiggleFactor)).toPoint(),
               QSize(d->wiggleFactor * 2, d->wiggleFactor * 2));
    if (!rect.contains(event->pos()))
        return;

    if (!d->canvas) {
        emit clicked(d->clickLocation);
    } else if (d->link.isEmpty()) {
        emit clicked(d->clickLocation);
    } else if (d->link.isRelative() && d->link.hasQuery()) {
        QUrlQuery query = QUrlQuery(d->link);
        if (query.hasQueryItem("page")) {
            bool ok;
            double top = query.queryItemValue("top").toFloat(&ok);
            if (!ok) top = -1.;
            double left = query.queryItemValue("left").toFloat(&ok);
            if (!ok) left = -1.;
            emit gotoClicked(query.queryItemValue("page").toInt(), top, left);
        } else {
            emit clicked(d->clickLocation);
        }
    } else {
        emit linkClicked(d->link);
    }
}

void PDFLinkArea::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit doubleClicked();
}

void PDFLinkArea::mouseUngrabEvent()
{
    d->pressTimer.stop();
    d->pressed = false;
    emit pressedChanged();
}

void PDFLinkArea::pressTimeout()
{
    emit longPress(d->clickLocation);
}
