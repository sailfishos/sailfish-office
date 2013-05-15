/*
 *
 */

#include "linkarea.h"

#include <QDebug>
#include <QtGui/QGraphicsSceneMouseEvent>

struct LinkLayerLink
{
    QRectF linkRect;
    QUrl linkTarget;
};

class LinkArea::Private
{
public:
    Private()
        : clickInProgress(false)
        , wiggleFactor(4)
    {}
    QVariantList links;
    QList<LinkLayerLink> realLinks;

    bool clickInProgress;
    QPointF clickLocation;
    int wiggleFactor;
};

LinkArea::LinkArea(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
    , d(new Private)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
    setAcceptTouchEvents(true);
}

LinkArea::~LinkArea()
{
    delete d;
}

QVariantList LinkArea::links() const
{
    return d->links;
}

void LinkArea::setLinks(const QVariantList& newLinks)
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
}

void LinkArea::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    d->clickInProgress = true;
    d->clickLocation = event->pos();
}

void LinkArea::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    d->clickInProgress = false;
    // Don't activate anything if the finger has moved too far
    QRect rect((d->clickLocation - QPointF(d->wiggleFactor, d->wiggleFactor)).toPoint(), QSize(d->wiggleFactor * 2, d->wiggleFactor * 2));
    if(!rect.contains(event->pos().toPoint())) {
        return;
    }
    QUrl url;
    foreach(const LinkLayerLink& link, d->realLinks) {
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
}

void LinkArea::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    emit doubleClicked();
}
