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
#include "pdfselection.h"
#include <QUrlQuery>
#include <QTimer>

class PDFLinkArea::Private
{
public:
    Private()
        : canvas(nullptr)
        , selection(nullptr)
        , wiggleFactor(4)
        , pressed(false)
        , annotation(nullptr)
        , clickOnSelection(false)
    { }

    PDFCanvas *canvas;
    PDFSelection *selection;

    QPointF clickLocation;
    int wiggleFactor;

    QTimer pressTimer;
    bool pressed;
    PDFCanvas::ReducedBox pressedBox;

    QUrl link;
    Poppler::Annotation *annotation;
    bool clickOnSelection;
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

PDFSelection* PDFLinkArea::selection() const
{
    return d->selection;
}

void PDFLinkArea::setSelection(PDFSelection *newSelection)
{
    if (newSelection != d->selection) {
        d->selection = newSelection;
        emit selectionChanged();
    }
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
    d->annotation = nullptr;
    d->clickOnSelection = false;

    d->clickLocation = event->pos();
    d->pressTimer.start();

    if (!d->canvas)
        return;
    
    // Click action logic in order.
    // - click on selection;
    // - unselect if selection is set;
    // - click on annotation;
    // - click on link;
    // - click.

    if (d->selection)
        d->clickOnSelection = d->selection->selectionAtPoint(d->clickLocation);
    if (d->clickOnSelection || (d->selection && d->selection->count() > 0))
        return;

    QPair<Poppler::Annotation *, PDFCanvas::ReducedBox> annotationAt =
        d->canvas->annotationAtPoint(d->clickLocation);
    d->annotation = annotationAt.first;
    if (annotationAt.first != nullptr) {
        d->pressedBox = annotationAt.second;
        d->pressed = true;
        emit pressedChanged();
        emit clickedBoxChanged();
        return;
    }

    QPair<QUrl, PDFCanvas::ReducedBox> urlAt = d->canvas->urlAtPoint(d->clickLocation);
    d->link = urlAt.first;
    if (!d->link.isEmpty()) {
        d->pressedBox = urlAt.second;
        d->pressed = true;
        emit pressedChanged();
        emit clickedBoxChanged();
        return;
    }
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

PDFAnnotation* PDFLinkArea::newProxyForAnnotation()
{
    // proxy will be child of this object to avoid memory leak.
    // Todo: transfer ownership to QML, if possible. Before that,
    //       all created proxy objects will be alive up to the moment
    //       the document object is released.
    switch (d->annotation->subType()) {
    case Poppler::Annotation::SubType::AText: {
        return new PDFTextAnnotation
            (static_cast<Poppler::TextAnnotation*>(d->annotation),
             d->canvas->document(), d->pressedBox.first, this);
    }
    case Poppler::Annotation::SubType::ACaret: {
        return new PDFCaretAnnotation
            (static_cast<Poppler::CaretAnnotation*>(d->annotation),
             d->canvas->document(), d->pressedBox.first, this);
    }
    case Poppler::Annotation::SubType::AHighlight: {
        return new PDFHighlightAnnotation
            (static_cast<Poppler::HighlightAnnotation*>(d->annotation),
             d->canvas->document(), d->pressedBox.first, this);
    }
    default: {
        return new PDFAnnotation
            (d->annotation, d->canvas->document(), d->pressedBox.first, this);
    }
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

    // Click action logic in order.
    // - click on selection;
    // - unselect if selection is set;
    // - click on annotation;
    // - click on link;
    // - click.

    if (!d->canvas) {
        emit clicked(d->clickLocation);
    } else if (d->clickOnSelection) {
        emit selectionClicked();
    } else if (d->selection && d->selection->count() > 0) {
        d->selection->unselect();
    } else if (d->annotation != nullptr) {
        emit annotationClicked(newProxyForAnnotation());
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
    if (!d->canvas)
        return;

    if (d->annotation) {
        emit annotationLongPress(newProxyForAnnotation());
    } else if (d->selection && d->selection->selectAt(d->clickLocation)) {
        return;
    } else {
        // Generic longPress.
        emit longPress(d->clickLocation);
    }
}
