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
        d->canvas = newCanvas;
        emit canvasChanged();
    }
}


void PDFLinkArea::mousePressEvent(QMouseEvent *event)
{
    d->clickLocation = event->pos();
    d->pressTimer.start();
}

void PDFLinkArea::mouseMoveEvent(QMouseEvent *event)
{
    // Don't activate anything if the finger has moved too far
    QRect rect((d->clickLocation - QPointF(d->wiggleFactor, d->wiggleFactor)).toPoint(),
               QSize(d->wiggleFactor * 2, d->wiggleFactor * 2));
    if (!rect.contains(event->pos())) {
        d->pressTimer.stop();
        return;
    }
}

void PDFLinkArea::mouseReleaseEvent(QMouseEvent *event)
{
    // Don't activate click if the longPress already fired.
    if (!d->pressTimer.isActive())
        return;
    d->pressTimer.stop();

    // Don't activate anything if the finger has moved too far
    QRect rect((d->clickLocation - QPointF(d->wiggleFactor, d->wiggleFactor)).toPoint(),
               QSize(d->wiggleFactor * 2, d->wiggleFactor * 2));
    if (!rect.contains(event->pos())) {
        return;
    }

    QUrl url;
    if (d->canvas)
        url = d->canvas->urlAtPoint(event->pos());

    if (url.isEmpty()) {
        emit clicked();
    } else if (url.isRelative() && url.hasQuery()) {
        QUrlQuery query = QUrlQuery(url);
        if (query.hasQueryItem("page")) {
            bool ok;
            double top = query.queryItemValue("top").toFloat(&ok);
            if (!ok) top = -1.;
            double left = query.queryItemValue("left").toFloat(&ok);
            if (!ok) left = -1.;
            emit gotoClicked(query.queryItemValue("page").toInt(), top, left);
        } else {
            emit clicked();
        }
    } else {
        emit linkClicked(url);
    }
}

void PDFLinkArea::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit doubleClicked();
}

void PDFLinkArea::mouseUngrabEvent()
{
    d->pressTimer.stop();
}

void PDFLinkArea::pressTimeout()
{
    emit longPress(d->clickLocation);
}
