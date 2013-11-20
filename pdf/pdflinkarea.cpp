/*
 *
 */

#include "pdflinkarea.h"
#include "pdfcanvas.h"

class PDFLinkArea::Private
{
public:
    Private()
        : canvas{ nullptr }
        , clickInProgress(false)
        , wiggleFactor(4)
    { }

    PDFCanvas* canvas;

    bool clickInProgress;
    QPointF clickLocation;
    int wiggleFactor;
};

PDFLinkArea::PDFLinkArea(QQuickItem* parent)
    : QQuickItem(parent)
    , d(new Private)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
}

PDFLinkArea::~PDFLinkArea()
{
    delete d;
}

PDFCanvas* PDFLinkArea::canvas() const
{
    return d->canvas;
}

void PDFLinkArea::setCanvas(PDFCanvas* newCanvas)
{
    if( newCanvas != d->canvas )
    {
        d->canvas = newCanvas;
        emit canvasChanged();
    }
}


void PDFLinkArea::mousePressEvent(QMouseEvent* event)
{
    d->clickInProgress = true;
    d->clickLocation = event->pos();
}

void PDFLinkArea::mouseReleaseEvent(QMouseEvent* event)
{
    d->clickInProgress = false;
    // Don't activate anything if the finger has moved too far
    QRect rect((d->clickLocation - QPointF(d->wiggleFactor, d->wiggleFactor)).toPoint(), QSize(d->wiggleFactor * 2, d->wiggleFactor * 2));
    if(!rect.contains(event->pos())) {
        return;
    }

    QUrl url;
    if( d->canvas )
        url = d->canvas->urlAtPoint( event->pos() );

    if(url.isEmpty()) {
        emit clicked();
    }
    else {
        emit linkClicked(url);
    }
    event->accept();
}

void PDFLinkArea::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit doubleClicked();
}
