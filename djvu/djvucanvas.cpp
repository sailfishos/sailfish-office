/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Copyright (C) 2022 Yura Beznos <yura.beznos@you-ra.info>
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

#include "djvucanvas.h"

#include <QtMath>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtGui/QPainter>
#include <QtQuick/QSGTransformNode>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleRectNode>
#include <QtQuick/QSGTexture>

//#include "djvu.h"
#include "djvurenderthread.h"
#include "djvudocument.h"

struct DJVUPage {
    DJVUPage()
        : index(-1)
        , requestId(0)
        , renderWidth(0)
        , requestWidth(0)
        , texture(nullptr)
        , linksRequested(false)
        , hasImage(false)
        , pageIsInCacheArea(false)
    { }

    int index;
    QRectF rect;

    int requestId;

    int renderWidth;
    int requestWidth;

    QRect textureArea;
    QImage image;
    QSGTexture *texture;

    bool linksRequested;
    bool hasImage;
    bool pageIsInCacheArea;
    DJVUDocument::LinkList links;
};

class DJVUCanvas::Private
{
public:
    Private(DJVUCanvas *qq)
        : q(qq)
        , pageSizeRequested(false)
        , pageCount(0)
        , currentPage(1)
        , renderWidth(0)
        , layoutScheduled(true)
        , document(nullptr)
        , flickable(0)
        , resizeTimer(nullptr)
        , spacing(10.f)
        , linkWiggle(4.f)
    { }

    enum TextureType{
        RootTexture,
        PatchTexture
    };

    DJVUCanvas *q;

    QVector<DJVUPage> pages;
    bool pageSizeRequested;

    int pageCount;
    int currentPage;

    int renderWidth;

    bool layoutScheduled;

    QPointer<DJVUDocument> document;
    QPointer<QQuickItem> flickable;

    QTimer *resizeTimer;

    float spacing;
    float linkWiggle;

    QRectF visibleArea;

    QList<QSizeF> pageSizes;

    QColor linkColor;
    QColor pagePlaceholderColor;

    void deleteAllTextures() {

        // Delete textures currently stored by pages.
        for (DJVUPage &page : pages) {
            if (page.requestId != 0 && document) {
                document->cancelPageRequest(page.requestId);
            }

            page.image = QImage();
            page.requestId = 0;
            page.hasImage = false;
        }
    }

    void cancelAllRequests()
    {
        if (document) {
            for (DJVUPage &page : pages) {
                if (page.requestId != 0) {
                    document->cancelPageRequest(page.requestId);
                    page.requestId = 0;
                }
            }
        }
    }
};


DJVUCanvas::DJVUCanvas(QQuickItem *parent)
    : QQuickItem(parent), d(new Private(this))
{
    setFlag(ItemHasContents, true);

    d->resizeTimer = new QTimer;
    d->resizeTimer->setInterval(500);
    d->resizeTimer->setSingleShot(true);
    connect(d->resizeTimer, &QTimer::timeout, this, &DJVUCanvas::resizeTimeout);
}

DJVUCanvas::~DJVUCanvas()
{
    d->cancelAllRequests();

    delete d->resizeTimer;
    delete d;
}

QQuickItem * DJVUCanvas::flickable() const
{
    return d->flickable;
}


static void connectUpdate(
        const QMetaObject *metaObject, QQuickItem *flickable, const char *propertyName, DJVUCanvas *canvas, const QMetaMethod &update)
{
    const int propertyIndex = metaObject->indexOfProperty(propertyName);
    if (propertyIndex != -1) {
        const QMetaMethod notify = metaObject->property(propertyIndex).notifySignal();

        if (notify.isValid()) {
            QObject::connect(flickable, notify, canvas, update);
        }
    }
}

void DJVUCanvas::setFlickable(QQuickItem *f)
{
    if (f != d->flickable) {
        if (d->flickable)
            d->flickable->disconnect(this);

        d->flickable = f;

        if (d->flickable) {
            const QMetaMethod schedulePolish = staticMetaObject.method(staticMetaObject.indexOfSlot("schedulePolish()"));
            const QMetaObject *metaObject = d->flickable->metaObject();

            connectUpdate(metaObject, d->flickable, "contentX", this, schedulePolish);
            connectUpdate(metaObject, d->flickable, "contentY", this, schedulePolish);

            connect(d->flickable, &QQuickItem::widthChanged, this, &DJVUCanvas::schedulePolish);
        }

        emit flickableChanged();
    }
}

DJVUDocument* DJVUCanvas::document() const
{
    return d->document;
}

void DJVUCanvas::setDocument(DJVUDocument *doc)
{
    if (doc != d->document) {
        if (d->document) {
            d->document->disconnect(this);
            d->pageSizes.clear();
        }

        d->document = doc;

        if (d->document) {
            connect(d->document.data(), &DJVUDocument::documentLoadedChanged, this, &DJVUCanvas::documentLoaded);
            //connect(d->document.data(), &DJVUDocument::linksFinished, this, &DJVUCanvas::linksFinished);
            connect(d->document.data(), &DJVUDocument::pageFinished, this, &DJVUCanvas::pageFinished);
            connect(d->document.data(), &DJVUDocument::pageSizesFinished, this, &DJVUCanvas::pageSizesFinished);
            //connect(d->document.data(), &DJVUDocument::documentLockedChanged, this, &DJVUCanvas::documentLoaded);
            //connect(d->document.data(), &DJVUDocument::pageModified, this, &DJVUCanvas::pageModified);

            if (d->document->isLoaded())
                documentLoaded();
        }

        emit documentChanged();
    }
}

QRectF DJVUCanvas::pageRectangle(int index) const
{
    if (index < 0 || index >= d->pages.count())
        return QRectF();

    return d->pages.at(index).rect;
}

int DJVUCanvas::currentPage() const
{
    return d->currentPage;
}

float DJVUCanvas::spacing() const
{
    return d->spacing;
}

void DJVUCanvas::setSpacing(float newValue)
{
    if (newValue != d->spacing) {
        d->spacing = newValue;
        emit spacingChanged();
    }
}
/*
float DJVUCanvas::linkWiggle() const
{
    return d->linkWiggle;
}

void DJVUCanvas::setLinkWiggle(float newValue)
{
    if (newValue != d->linkWiggle) {
        d->linkWiggle = newValue;
        emit linkWiggleChanged();
    }
}

QColor DJVUCanvas::linkColor() const
{
    return d->linkColor;
}

void DJVUCanvas::setLinkColor(const QColor &color)
{
    if (color != d->linkColor) {
        d->linkColor = color;
        d->linkColor.setAlphaF(0.25);
        update();
        emit linkColorChanged();
    }
}
*/
QColor DJVUCanvas::pagePlaceholderColor() const
{
    return d->pagePlaceholderColor;
}

void DJVUCanvas::setPagePlaceholderColor(const QColor &color)
{
    if (color != d->pagePlaceholderColor) {
        d->pagePlaceholderColor = color;
        d->pagePlaceholderColor.setAlphaF(0.25);
        update();
        emit pagePlaceholderColorChanged();
    }
}

void DJVUCanvas::layout()
{
    d->layoutScheduled = false;

    if (d->pageSizes.count() == 0) {
        if (d->document && d->document->isLoaded() && d->pageCount > 0 && !d->pageSizeRequested) {
            d->document->requestPageSizes();
            d->pageSizeRequested = true;
        }
        return;
    }

    for (int i = d->pageCount; i < d->pages.count(); ++i) {
        DJVUPage &page = d->pages[i];

        if (page.requestId != 0) {
            d->document->cancelPageRequest(page.requestId);
        }
    }

    d->pages.resize(d->pageCount);

    float totalHeight = 0.f;
    for (int i = 0; i < d->pages.count(); ++i) {
        QSizeF unscaledSize = d->pageSizes.at(i);
        float ratio = unscaledSize.height() / unscaledSize.width();

        DJVUPage &page = d->pages[i];
        page.index = i;
        page.rect = QRectF(0, totalHeight, width(), width() * ratio);

        totalHeight += page.rect.height();
        if (i < d->pageCount - 1)
            totalHeight += d->spacing;
    }

    setHeight(int(totalHeight));

    emit pageLayoutChanged();

    update();
}

qreal DJVUCanvas::squaredDistanceFromRect(const QRectF &pageRect,
                                         const QRectF &reducedCoordRect,
                                         const QPointF &point) const
{
    qreal dist = 0.;
    QRectF rect {
        reducedCoordRect.x() * pageRect.width(),
        reducedCoordRect.y() * pageRect.height() + pageRect.y(),
        reducedCoordRect.width() * pageRect.width(),
        reducedCoordRect.height() * pageRect.height()
    };

    if ((qreal)point.x() < rect.left()) {
        dist += (rect.left() - (qreal)point.x()) * (rect.left() - (qreal)point.x());
    } else if ((qreal)point.x() > rect.right()) {
        dist += (rect.right() - (qreal)point.x()) * (rect.right() - (qreal)point.x());
    }
    if ((qreal)point.y() < rect.top()) {
        dist += (rect.top() - (qreal)point.y()) * (rect.top() - (qreal)point.y());
    } else if ((qreal)point.y() > rect.bottom()) {
        dist += (rect.bottom() - (qreal)point.y()) * (rect.bottom() - (qreal)point.y());
    }

    return dist;
}

QPair<QUrl, DJVUCanvas::ReducedBox> DJVUCanvas::urlAtPoint(const QPointF &point) const
{
    for (int i = 0; i < d->pages.count(); ++i) {
        const DJVUPage &page = d->pages.at(i);
        if (page.rect.contains(point)) {
            qreal squaredDistanceMin = d->linkWiggle * d->linkWiggle;
            QUrl url;
            QRectF at;
            for (const QPair<QRectF, QUrl> &link : page.links) {
                qreal squaredDistance =
                    squaredDistanceFromRect(page.rect, link.first, point);
                
                if (squaredDistance < squaredDistanceMin) {
                    url = link.second;
                    at = link.first;
                    squaredDistanceMin = squaredDistance;
                }
            }
            return QPair<QUrl, DJVUCanvas::ReducedBox> {url, {i, at}};
        }
    }

    return QPair<QUrl, DJVUCanvas::ReducedBox>();
}
/*
QPair<Poppler::Annotation*, DJVUCanvas::ReducedBox> DJVUCanvas::annotationAtPoint(const QPointF &point) const
{
    for (int i = 0; i < d->pages.count(); ++i) {
        const DJVUPage &page = d->pages.at(i);
        if (page.rect.contains(point)) {
            qreal squaredDistanceMin = d->linkWiggle * d->linkWiggle;
            Poppler::Annotation *result = nullptr;
            QRectF at;
            for (Poppler::Annotation *annotation : d->document->annotations(i)) {
                switch (annotation->subType()) {
                case (Poppler::Annotation::ALink):
                    // Ignore link annotation for the moment since
                    // real link are reported as annotation also.
                    break;
                case (Poppler::Annotation::AHighlight): {
                    QList<Poppler::HighlightAnnotation::Quad> quads =
                        static_cast<Poppler::HighlightAnnotation*>(annotation)->highlightQuads();
                    for (QList<Poppler::HighlightAnnotation::Quad>::iterator quad = quads.begin();
                         quad != quads.end(); quad++) {
                        // Assuming rectangular quad...
                        qreal squaredDistance =
                            squaredDistanceFromRect(page.rect, QRectF(quad->points[0], quad->points[2]), point);
                
                        if (squaredDistance < squaredDistanceMin) {
                            result = annotation;
                            at = QRectF(quad->points[0], quad->points[2]);
                            squaredDistanceMin = squaredDistance;
                        }
                    }
                    break;
                }
                default: {
                    qreal squaredDistance =
                        squaredDistanceFromRect(page.rect, annotation->boundary(), point);
                
                    if (squaredDistance < squaredDistanceMin) {
                        result = annotation;
                        at = annotation->boundary();
                        squaredDistanceMin = squaredDistance;
                    }
                    break;
                }
                }
            }
            return QPair<Poppler::Annotation*, DJVUCanvas::ReducedBox>{result, {i, at}};
        }
    }

    return QPair<Poppler::Annotation*, DJVUCanvas::ReducedBox>();
}
*/
QRectF DJVUCanvas::fromPageToItem(int index, const QRectF &rect) const
{
    if (index < 0 || index >= d->pages.count())
        return QRectF();

    const DJVUPage &page = d->pages.at(index);
    return QRectF(rect.x() * page.rect.width() + page.rect.x(),
                  rect.y() * page.rect.height() + page.rect.y(),
                  rect.width() * page.rect.width(),
                  rect.height() * page.rect.height());
}

QPointF DJVUCanvas::fromPageToItem(int index, const QPointF &point) const
{
    if (index < 0 || index >= d->pages.count())
        return QPointF();

    const DJVUPage &page = d->pages.at(index);
    return QPointF(point.x() * page.rect.width() + page.rect.x(),
                   point.y() * page.rect.height() + page.rect.y());
}
/*
void DJVUCanvas::linksFinished(int id, const QList<QPair<QRectF, QUrl> > &links)
{
    if (id < 0 || id >= d->pages.count()) {
        return;
    }

    DJVUPage &page = d->pages[id];

    page.links = links;
    if (!page.links.isEmpty())
        update();
}
*/
void DJVUCanvas::pageModified(int id, const QRectF &subpart)
{
    if (id < 0 || id >= d->pages.count()) {
        return;
    }

    DJVUPage &page = d->pages[id];

    // Request an update of the page if it has been loaded.
    if (page.hasImage || page.requestId != 0) {
        page.image = QImage();
        page.hasImage = false;

        if (page.requestId != 0) {
            d->document->cancelPageRequest(page.requestId);
            page.requestId = 0;
        }
    }
    polish();
}

void DJVUCanvas::pageFinished(int requestId, int pageRenderWidth,
                             QRect subpart, const QImage &image, int extraData)
{
    auto it = std::find_if(d->pages.begin(), d->pages.end(), [&](const DJVUPage &page) {
        return page.requestId == requestId;
    });
    if (it != d->pages.end()) {
        DJVUPage &page = *it;

        page.requestId = 0;
        page.renderWidth = pageRenderWidth;
        page.textureArea = subpart;
        page.image = image;
        page.hasImage = true;

        update();

        polish();
    }
}

void DJVUCanvas::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (oldGeometry.width() != newGeometry.width()) {
        d->resizeTimer->start();
        layout();
    }
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

// QQuickWindow will invoke a slot with this name directly if needed, no need to connect.
void DJVUCanvas::invalidateSceneGraph()
{
    d->deleteAllTextures();
}

static void putTexture(QQuickWindow *window, QSGSimpleTextureNode *tn, const QImage &image)
{
    tn->setOwnsTexture(true);
    tn->setTexture(window->createTextureFromImage(image));
}

static void setTextureRect(
        QSGSimpleTextureNode *tn, float pageWidth, int renderWidth, const QRect &textureArea)
{
    if (int(pageWidth) == renderWidth) {
        tn->setRect(textureArea);
    } else {
        float ratio = pageWidth / renderWidth;
        tn->setRect(int(ratio * textureArea.x()),
                    int(ratio * textureArea.y()),
                    qCeil(ratio * textureArea.width()),
                    qCeil(ratio * textureArea.height()));
    }
}

QSGNode* DJVUCanvas::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *)
{
    if (d->pageCount == 0) {
        delete node;
        return nullptr;
    }
    QQuickWindow * const window = this->window();

    QSGNode *root = static_cast<QSGNode*>(node);
    if (!root) {
        root = new QSGNode;
    }

    for (int i = 0; i < d->pages.count(); ++i) {
        DJVUPage &page = d->pages[i];
        QSGTransformNode *t = static_cast<QSGTransformNode*>(root->childAtIndex(i));
        if (!t) {
            t = new QSGTransformNode;
            t->setFlag(QSGNode::OwnedByParent);
            root->appendChildNode(t);
        }

        QMatrix4x4 m;
        m.translate(0, page.rect.y());
        t->setMatrix(m);

        if (page.pageIsInCacheArea) {
            // Node hierachy:
            // t
            // |-bg
            // |  |-tn
            // |  |-n
            // |  | |- link1
            // |  | |- link2...
            QSGSimpleRectNode *bg = static_cast<QSGSimpleRectNode*>(t->firstChild());
            if (!bg) {
                bg = new QSGSimpleRectNode;
                bg->setFlag(QSGNode::OwnedByParent);
                bg->setColor(d->pagePlaceholderColor);
                t->appendChildNode(bg);
            }
            bg->setRect(0., 0., page.rect.width(), page.rect.height());

            if (page.hasImage) {
                QSGSimpleTextureNode *tn = static_cast<QSGSimpleTextureNode *>(bg->firstChild());
                if (!tn) {
                    tn = new QSGSimpleTextureNode;
                    tn->setFlag(QSGNode::OwnedByParent);
                    bg->appendChildNode(tn);
                }
                if (!page.image.isNull()) {
                    putTexture(window, tn, page.image);
                    page.image = QImage();
                }
                setTextureRect(tn, width(), page.renderWidth, page.textureArea);

                QSGNode *n = tn->nextSibling();
                if (!n) {
                    n = new QSGNode;
                    n->setFlag(QSGNode::OwnedByParent);
                    bg->appendChildNode(n);
                }
                QSGSimpleRectNode *rn = static_cast<QSGSimpleRectNode*>(n->firstChild());
                for (int l = 0; l < page.links.count(); ++l) {
                    if (!rn) {
                        rn = new QSGSimpleRectNode;
                        rn->setFlag(QSGNode::OwnedByParent);
                        n->appendChildNode(rn);
                    }
                    QRectF linkRect = page.links.value(l).first;
                    QRectF targetRect{
                        linkRect.x() * page.rect.width(),
                        linkRect.y() * page.rect.height(),
                        linkRect.width() * page.rect.width(),
                        linkRect.height() * page.rect.height()
                    };
                    rn->setRect(targetRect);
                    rn->setColor(d->linkColor);

                    rn = static_cast<QSGSimpleRectNode*>(rn->nextSibling());
                }
            } else {
                delete bg->firstChild(); // delete the texture root here.
                delete bg->firstChild(); // delete the link root here.
            }
        } else {
            delete t->firstChild();
        }
    }

    return root;
}

void DJVUCanvas::updatePolish()
{
    if (d->layoutScheduled) {
        layout();
    }

    if (!d->flickable) {
        return;
    }

    QQuickWindow * const window = this->window();

    //Visible area equals flickable translated by contentX/Y
    const QRectF visibleArea = mapRectFromItem(d->flickable, d->flickable->boundingRect());

    //Loaded area equals visible area scaled to five times the size
    const QRectF loadedArea = {
        visibleArea.x() - visibleArea.width() * 2,
        visibleArea.y() - visibleArea.height() * 2,
        visibleArea.width() * 5,
        visibleArea.height() * 5,
    };
    const QRect textureLimit = {
        0, 0,
        int(2.5 * qMin(window->width(), window->height())),
        int(2.5 * qMin(window->width(), window->height()))
    };
    const float renderingRatio = float(d->renderWidth) / width();

    struct PriorityRequest { int index; int width; QRect area; };

    QVector<PriorityRequest> priorityRequests;
    int currentPage = d->currentPage;
    qreal maxVisibleArea = 0.;

    for (int i = 0; i < d->pages.count(); ++i) {
        DJVUPage &page = d->pages[i];

        page.pageIsInCacheArea = page.rect.intersects(loadedArea);

        const bool pageIsInVisibleArea = page.rect.intersects(visibleArea);

        // Current rendering in pixels is done with a width of
        // d->renderWidth which can be different than actual width()
        // when zooming.
        QRect pageRect = {
            0, 0, d->renderWidth, int(page.rect.height() * renderingRatio)
        };

        if (pageIsInVisibleArea) {/*
            if (!page.linksRequested) {
                page.linksRequested = true;
                d->document->requestLinksAtPage(i);
            }
*/
            const bool fullPageFit = textureLimit.contains(pageRect);
            QRect showableArea = {
                int(renderingRatio * (visibleArea.x() - float(window->width() / 4.) - page.rect.x())),
                int(renderingRatio * (visibleArea.y() - float(window->height() / 4.) - page.rect.y())),
                int(renderingRatio * (visibleArea.width() + float(window->width() / 2.))),
                int(renderingRatio * (visibleArea.height() + float(window->height() / 2.)))
            };

            QRect requestArea = textureLimit;
            requestArea.moveCenter(showableArea.center());

            showableArea = showableArea.intersected(pageRect);
            // Limit showableArea with requestArea to avoid looping request
            // for something too big.
            showableArea = showableArea.intersected(requestArea);

            if (!page.hasImage
                || page.requestWidth != d->renderWidth
                || (page.requestWidth == int(width()) &&
                    !page.textureArea.contains(showableArea))) {
                QRect request = (fullPageFit) ? QRect() : requestArea;
                if (page.requestId == 0 || d->document->cancelPageRequest(page.requestId)) {
                    page.requestId = d->document->requestPage(i, d->renderWidth, request);
                    page.requestWidth = d->renderWidth;
                }

                PriorityRequest priorityRequest = { i, d->renderWidth, request };
                priorityRequests.append(priorityRequest);
            }

            QRectF inter = page.rect.intersected(visibleArea);
            qreal area = inter.width() * inter.height();
            // Select the current page as the page with the maximum
            // visible area.
            if (area > maxVisibleArea) {
                maxVisibleArea = area;
                currentPage = i + 1;
            }
        } else if (page.pageIsInCacheArea && textureLimit.contains(pageRect)) {
            // We preload full page only if they can fit into texture.
            if (page.requestWidth != d->renderWidth
                    && (page.requestId == 0 || d->document->cancelPageRequest(page.requestId))) {
                page.requestId = d->document->requestPage(i, d->renderWidth, QRect());
                page.requestWidth = d->renderWidth;
            }
        } else if (page.requestId != 0 || page.hasImage) {
            // Scrolled beyond where this page is needed, skip it.
            if (page.requestId != 0) {
                d->document->cancelPageRequest(page.requestId);
                page.requestId = 0;
            }

            page.image = QImage();
            page.hasImage = false;
            page.requestWidth = 0;

            update();
        }
    }

    // prioritize in reverse order so we end up with a final priority list which is
    // ordered by increasing page number.
    for (PriorityRequest &request : priorityRequests) {
        d->document->prioritizeRequest(request.index, request.width, request.area);
    }

    if (d->currentPage != currentPage) {
        d->currentPage = currentPage;
        emit currentPageChanged();
    }
}

void DJVUCanvas::itemChange(ItemChange change, const ItemChangeData &data)
{
    switch (change) {
    case ItemVisibleHasChanged:
        if (data.boolValue) {
            polish();
        }
        break;
    default:
        break;
    }

    QQuickItem::itemChange(change, data);
}

void DJVUCanvas::documentLoaded()
{
    d->pages.clear();
    d->pageCount = d->document->pageCount();
    d->renderWidth = width();
    d->layoutScheduled = true;
    polish();
}

void DJVUCanvas::resizeTimeout()
{
    d->renderWidth = width();
    polish();
}

void DJVUCanvas::pageSizesFinished(const QList<QSizeF> &sizes)
{
    d->pageSizes = sizes;
    d->pageSizeRequested = false;
    d->layoutScheduled = true;
    polish();
}

QPair<int, QRectF> DJVUCanvas::pageAtPoint(const QPointF &point) const
{
    for (int i = 0; i < d->pages.count(); ++i) {
        const DJVUPage& page = d->pages.at(i);
        if (page.rect.contains(point)) {
            return QPair<int, QRectF>{i, page.rect};
        }
    }
    return QPair<int, QRectF>{-1, QRectF()};
}

