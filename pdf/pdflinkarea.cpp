/*
 *
 */

#include "pdflinkarea.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>

struct LinkLayerLink
{
    QRectF linkRect;
    QUrl linkTarget;
};

class PDFLinkArea::Private
{
public:
    Private()
        : clickInProgress(false)
        , wiggleFactor(4)
    {
        linkColor = Qt::blue;
    }
    QVariantList links;
    QList<LinkLayerLink> realLinks;

    bool clickInProgress;
    QPointF clickLocation;
    int wiggleFactor;

    QSizeF sourceSize;

    QColor linkColor;
};

PDFLinkArea::PDFLinkArea(QQuickItem* parent)
    : QQuickItem(parent)
    , d(new Private)
{
    setFlag(ItemHasContents);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
//    setAcceptTouchEvents(true);
}

PDFLinkArea::~PDFLinkArea()
{
    delete d;
}

// void PDFLinkArea::paint(QPainter* painter)
// {
//     painter->setBrush(Qt::transparent);
//     painter->setPen(d->linkColor);
//     foreach(const LinkLayerLink& link, d->realLinks) {
//         QRectF target(
//             (link.linkRect.y() / d->sourceSize.height()) * height(),
//             (link.linkRect.x() / d->sourceSize.width()) * width(),
//             (link.linkRect.height() / d->sourceSize.height()) * height(),
//             (link.linkRect.width() / d->sourceSize.width()) * width());
//         painter->drawRect(link.linkRect);
//     }
// }

QVariantList PDFLinkArea::links() const
{
    return d->links;
}

void PDFLinkArea::setLinks(const QVariantList& newLinks)
{
    d->links = newLinks;
    // run through the new data and cache a data list with the information
    // so we don't have to interpret the QObjects all the time
    d->realLinks.clear();
    foreach(const QVariant& var, newLinks)
    {
        QObject* obj = var.value<QObject*>();
        if(!obj)
            continue;
        LinkLayerLink link;
        link.linkRect = obj->property("linkRect").toRectF().adjusted(-d->wiggleFactor, -d->wiggleFactor, d->wiggleFactor, d->wiggleFactor);
        link.linkTarget = obj->property("linkTarget").toUrl();
        d->realLinks.append(link);
    }
    emit linksChanged();
    update();
}

QSizeF PDFLinkArea::sourceSize() const
{
    return d->sourceSize;
}

void PDFLinkArea::setSourceSize(const QSizeF& size)
{
    if(size != d->sourceSize) {
        d->sourceSize = size;
        emit sourceSizeChanged();
        update();
    }
}

QColor PDFLinkArea::linkColor() const
{
    return d->linkColor;
}

void PDFLinkArea::setLinkColor(const QColor& color)
{
    if(color != d->linkColor) {
        d->linkColor = color;
        d->linkColor.setAlphaF( 0.25 );
        emit linkColorChanged();
        update();
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
    QPointF inverted(event->pos().y(), event->pos().x());
    foreach(const LinkLayerLink& link, d->realLinks) {
        QRectF scaledTarget(
            (link.linkRect.x() / d->sourceSize.width()) * width(),
            (link.linkRect.y() / d->sourceSize.height()) * height(),
            (link.linkRect.width() / d->sourceSize.width()) * width(),
            (link.linkRect.height() / d->sourceSize.height()) * height() );

        if(link.linkRect.contains(event->pos())) {
            url = link.linkTarget;
            break;
        }
    }
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
