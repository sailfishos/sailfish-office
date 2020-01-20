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

#include "pdfcanvas.h"

#include <QtMath>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtGui/QPainter>
#include <QtQuick/QSGTransformNode>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleRectNode>
#include <QtQuick/QSGTexture>

#include "pdfrenderthread.h"
#include "pdfdocument.h"

typedef QPair<QRect, QImage> Patch;

struct PDFPage {
    PDFPage()
        : index(-1)
        , requestId(0)
        , renderWidth(0)
        , texture(nullptr)
        , linksLoaded(false)
        , hasImage(false)
    { }

    int index;
    QRectF rect;

    int requestId;

    int renderWidth;

    QRect textureArea;
    QImage image;
    QSGTexture *texture;

    QList<Patch> patches;

    bool linksLoaded;
    bool hasImage;
    PDFDocument::LinkList links;
};

class PDFCanvas::Private
{
public:
    Private(PDFCanvas *qq)
        : q(qq)
        , pageSizeRequested(false)
        , pageCount(0)
        , currentPage(1)
        , renderWidth(0)
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

    PDFCanvas *q;

    QHash<int, PDFPage> pages;
    bool pageSizeRequested;

    int pageCount;
    int currentPage;

    int renderWidth;

    QPointer<PDFDocument> document;
    QPointer<QQuickItem> flickable;

    QTimer *resizeTimer;

    float spacing;
    float linkWiggle;

    QRectF visibleArea;

    QList<QSizeF> pageSizes;

    QColor linkColor;
    QColor pagePlaceholderColor;

    QPointer<QQuickWindow> connectedWindow;

    void deleteAllTextures() {

        // Delete textures currently stored by pages.
        for (PDFPage &page : pages) {
            if (page.requestId != 0 && document) {
                document->cancelPageRequest(page.requestId);
            }

            page.image = QImage();
            page.patches.clear();
            page.requestId = 0;
            page.hasImage = false;
        }
    }

    void cancelAllRequests()
    {
        if (document) {
            for (PDFPage &page : pages) {
                if (page.requestId != 0) {
                    document->cancelPageRequest(page.requestId);
                    page.requestId = 0;
                }
            }
        }
    }
};


PDFCanvas::PDFCanvas(QQuickItem *parent)
    : QQuickItem(parent), d(new Private(this))
{
    setFlag(ItemHasContents, true);

    d->resizeTimer = new QTimer;
    d->resizeTimer->setInterval(500);
    d->resizeTimer->setSingleShot(true);
    connect(d->resizeTimer, &QTimer::timeout, this, &PDFCanvas::resizeTimeout);
}

PDFCanvas::~PDFCanvas()
{
    d->cancelAllRequests();

    delete d->resizeTimer;
    delete d;
}

QQuickItem * PDFCanvas::flickable() const
{
    return d->flickable;
}

static void connectUpdate(
        const QMetaObject *metaObject, QQuickItem *flickable, const char *propertyName, PDFCanvas *canvas, const QMetaMethod &update)
{
    const int propertyIndex = metaObject->indexOfProperty(propertyName);
    if (propertyIndex != -1) {
        const QMetaMethod notify = metaObject->property(propertyIndex).notifySignal();

        if (notify.isValid()) {
            QObject::connect(flickable, notify, canvas, update);
        }
    }
}

void PDFCanvas::setFlickable(QQuickItem *f)
{
    if (f != d->flickable) {
        if (d->flickable)
            d->flickable->disconnect(this);

        d->flickable = f;

        if (d->flickable) {
            const QMetaMethod update = staticMetaObject.method(staticMetaObject.indexOfSlot("update()"));
            const QMetaObject *metaObject = d->flickable->metaObject();

            connectUpdate(metaObject, d->flickable, "contentX", this, update);
            connectUpdate(metaObject, d->flickable, "contentY", this, update);

            connect(d->flickable, &QQuickItem::widthChanged, this, &PDFCanvas::update);
        }

        emit flickableChanged();
    }
}

PDFDocument* PDFCanvas::document() const
{
    return d->document;
}

void PDFCanvas::setDocument(PDFDocument *doc)
{
    if (doc != d->document) {
        if (d->document) {
            d->document->disconnect(this);
            d->pageSizes.clear();
        }

        d->document = doc;

        if (d->document) {
            connect(d->document.data(), &PDFDocument::documentLoadedChanged, this, &PDFCanvas::documentLoaded);
            connect(d->document.data(), &PDFDocument::linksFinished, this, &PDFCanvas::linksFinished);
            connect(d->document.data(), &PDFDocument::pageFinished, this, &PDFCanvas::pageFinished);
            connect(d->document.data(), &PDFDocument::pageSizesFinished, this, &PDFCanvas::pageSizesFinished);
            connect(d->document.data(), &PDFDocument::documentLockedChanged, this, &PDFCanvas::documentLoaded);
            connect(d->document.data(), &PDFDocument::pageModified, this, &PDFCanvas::pageModified);

            if (d->document->isLoaded())
                documentLoaded();
        }

        emit documentChanged();
    }
}

QRectF PDFCanvas::pageRectangle(int index) const
{
    if (d->pages.count() == 0)
        return QRectF();

    return d->pages.value(index).rect;
}

int PDFCanvas::currentPage() const
{
    return d->currentPage;
}

float PDFCanvas::spacing() const
{
    return d->spacing;
}

void PDFCanvas::setSpacing(float newValue)
{
    if (newValue != d->spacing) {
        d->spacing = newValue;
        emit spacingChanged();
    }
}

float PDFCanvas::linkWiggle() const
{
    return d->linkWiggle;
}

void PDFCanvas::setLinkWiggle(float newValue)
{
    if (newValue != d->linkWiggle) {
        d->linkWiggle = newValue;
        emit linkWiggleChanged();
    }
}

QColor PDFCanvas::linkColor() const
{
    return d->linkColor;
}

void PDFCanvas::setLinkColor(const QColor &color)
{
    if (color != d->linkColor) {
        d->linkColor = color;
        d->linkColor.setAlphaF(0.25);
        update();
        emit linkColorChanged();
    }
}

QColor PDFCanvas::pagePlaceholderColor() const
{
    return d->pagePlaceholderColor;
}

void PDFCanvas::setPagePlaceholderColor(const QColor &color)
{
    if (color != d->pagePlaceholderColor) {
        d->pagePlaceholderColor = color;
        d->pagePlaceholderColor.setAlphaF(0.25);
        update();
        emit pagePlaceholderColorChanged();
    }
}

void PDFCanvas::layout()
{
    if (d->pageSizes.count() == 0) {
        if (d->document && d->document->isLoaded() && d->pageCount > 0 && !d->pageSizeRequested) {
            d->document->requestPageSizes();
            d->pageSizeRequested = true;
        }
        return;
    }

    float totalHeight = 0.f;
    for (int i = 0; i < d->pageCount; ++i) {
        QSizeF unscaledSize = d->pageSizes.at(i);
        float ratio = unscaledSize.height() / unscaledSize.width();


        PDFPage page;
        page.index = i;
        page.rect = QRectF(0, totalHeight, width(), width() * ratio);
        page.requestId = 0; // We're cancelling all requests below
        if (d->pages.contains(i)) {
            page.renderWidth = d->pages.value(i).renderWidth;
            page.textureArea = d->pages.value(i).textureArea;
            page.image = d->pages.value(i).image;
            page.patches = d->pages.value(i).patches;
            page.links = d->pages.value(i).links;
        }
        d->pages.insert(i, page);

        totalHeight += page.rect.height();
        if (i < d->pageCount - 1)
            totalHeight += d->spacing;
    }

    setHeight(int(totalHeight));

    // We're going to be requesting new images for all content, so remove
    // pending reuqests to minimize the delay before they come.
    d->cancelAllRequests();

    emit pageLayoutChanged();

    update();
}

qreal PDFCanvas::squaredDistanceFromRect(const QRectF &pageRect,
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

QPair<QUrl, PDFCanvas::ReducedBox> PDFCanvas::urlAtPoint(const QPointF &point) const
{
    for (int i = 0; i < d->pageCount; ++i) {
        const PDFPage &page = d->pages.value(i);
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
            return QPair<QUrl, PDFCanvas::ReducedBox> {url, {i, at}};
        }
    }

    return QPair<QUrl, PDFCanvas::ReducedBox>();
}

QPair<Poppler::Annotation*, PDFCanvas::ReducedBox> PDFCanvas::annotationAtPoint(const QPointF &point) const
{
    for (int i = 0; i < d->pageCount; ++i) {
        const PDFPage &page = d->pages.value(i);
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
            return QPair<Poppler::Annotation*, PDFCanvas::ReducedBox>{result, {i, at}};
        }
    }

    return QPair<Poppler::Annotation*, PDFCanvas::ReducedBox>();
}

QRectF PDFCanvas::fromPageToItem(int index, const QRectF &rect) const
{
    if (index < 0 || index >= d->pageCount)
        return QRectF();

    const PDFPage &page = d->pages.value(index);
    return QRectF(rect.x() * page.rect.width() + page.rect.x(),
                  rect.y() * page.rect.height() + page.rect.y(),
                  rect.width() * page.rect.width(),
                  rect.height() * page.rect.height());
}

QPointF PDFCanvas::fromPageToItem(int index, const QPointF &point) const
{
    if (index < 0 || index >= d->pageCount)
        return QPointF();

    const PDFPage &page = d->pages.value(index);
    return QPointF(point.x() * page.rect.width() + page.rect.x(),
                   point.y() * page.rect.height() + page.rect.y());
}

void PDFCanvas::linksFinished(int id, const QList<QPair<QRectF, QUrl> > &links)
{
    PDFPage &page = d->pages[id];

    page.linksLoaded = true;
    page.links = links;
    if (!page.links.isEmpty())
        update();
}

void PDFCanvas::pageModified(int id, const QRectF &subpart)
{
    PDFPage &page = d->pages[id];

    if (subpart.isEmpty()) {
        // Ask for a full page redraw in update by deleting
        // the current texture of the page.
        page.image = QImage();
        page.hasImage = false;
        if (page.requestId != 0) {
            d->document->cancelPageRequest(page.requestId);
            page.requestId = 0;
        }
        update();
    } else {
        int buf = 10;
        // Ask only for a patch on this page.
        QRect request(int(subpart.x() * page.rect.width()) - buf,
                      int(subpart.y() * page.rect.height()) - buf,
                      qCeil(subpart.width() * page.rect.width()) + buf * 2,
                      qCeil(subpart.height() * page.rect.height()) + buf * 2);
        page.requestId = d->document->requestPage(id, d->renderWidth,
                                 request, PDFCanvas::Private::PatchTexture);
    }
}

void PDFCanvas::pageFinished(int requestId, int pageRenderWidth,
                             QRect subpart, const QImage &image, int extraData)
{
    auto it = std::find_if(d->pages.begin(), d->pages.end(), [&](const PDFPage &page) {
        return page.requestId == requestId;
    });
    if (it != d->pages.end()) {
        PDFPage &page = *it;

        page.requestId = 0;

        if (PDFCanvas::Private::TextureType(extraData) == PDFCanvas::Private::RootTexture) {
            page.renderWidth = pageRenderWidth;
            page.textureArea = subpart;
            page.image = image;
            page.hasImage = true;
        } else if (pageRenderWidth == page.renderWidth) {
            page.patches.append(Patch(subpart, image));
        }

        update();
    }
}

void PDFCanvas::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (oldGeometry.width() != newGeometry.width()) {
        QMetaObject::invokeMethod(d->resizeTimer, "start");
        layout();
    }
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

void PDFCanvas::sceneGraphInvalidated()
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

QSGNode* PDFCanvas::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *)
{
    if (d->pageCount == 0 || !d->flickable) {
        delete node;
        return nullptr;
    }

    QQuickWindow * const window = this->window();
    if (window != d->connectedWindow) {
        d->connectedWindow = window;
        connect(window, &QQuickWindow::sceneGraphInvalidated, this, &PDFCanvas::sceneGraphInvalidated);
    }

    //Visible area equals flickable translated by contentX/Y
    QRectF visibleArea = mapRectFromItem(d->flickable, d->flickable->boundingRect());

    //Loaded area equals visible area scaled to five times the size
    QRectF loadedArea = {
        visibleArea.x() - visibleArea.width() * 2,
        visibleArea.y() - visibleArea.height() * 2,
        visibleArea.width() * 5,
        visibleArea.height() * 5,
    };
    QRect textureLimit = {
        0, 0,
        int(2.5 * qMin(window->width(), window->height())),
        int(2.5 * qMin(window->width(), window->height()))
    };
    float renderingRatio = float(d->renderWidth) / width();

    QSGNode *root = static_cast<QSGNode*>(node);
    if (!root) {
        root = new QSGNode;
    }

    QList<QPair<int, QPair<int, QRect> > > priorityRequests;
    int currentPage = d->currentPage;
    qreal maxVisibleArea = 0.;

    for (int i = 0; i < d->pageCount; ++i) {
        PDFPage &page = d->pages[i];

        bool loadPage = page.rect.intersects(loadedArea);
        bool showPage = page.rect.intersects(visibleArea);

        // Current rendering in pixels is done with a width of
        // d->renderWidth which can be different than actual width()
        // when zooming.
        QRect pageRect = {
            0, 0, d->renderWidth, int(page.rect.height() * renderingRatio)
        };

        if (showPage) {
            if (!page.linksLoaded)
                d->document->requestLinksAtPage(i);

            textureLimit.moveTo(0, 0);
            bool fullPageFit = textureLimit.contains(pageRect);
            QRect showableArea = {
                int(renderingRatio * (visibleArea.x() - float(window->width() / 4.) - page.rect.x())),
                int(renderingRatio * (visibleArea.y() - float(window->height() / 4.) - page.rect.y())),
                int(renderingRatio * (visibleArea.width() + float(window->width() / 2.))),
                int(renderingRatio * (visibleArea.height() + float(window->height() / 2.)))
            };
            showableArea = showableArea.intersected(pageRect);
            // Limit showableArea with textureLimit to avoid looping request
            // for something too big.
            textureLimit.moveCenter(showableArea.center());
            showableArea = showableArea.intersected(textureLimit);

            if (!page.hasImage
                || page.renderWidth != d->renderWidth
                || (page.renderWidth == int(width()) &&
                    !page.textureArea.contains(showableArea))) {
                QRect request = (fullPageFit) ? QRect() : textureLimit;
                if (page.requestId == 0) {
                    page.requestId = d->document->requestPage(i, d->renderWidth,
                                             request, PDFCanvas::Private::RootTexture);
                }
                priorityRequests << QPair<int, QPair<int, QRect> >(i, QPair<int, QRect>(d->renderWidth, request));
            }
        } else if (loadPage
                   && page.requestId == 0 && (page.renderWidth != d->renderWidth)) {
            textureLimit.moveTo(0, 0);
            // We preload full page only if they can fit into texture.
            if (textureLimit.contains(pageRect)) {
                page.requestId = d->document->requestPage(i, d->renderWidth,
                                         QRect(), PDFCanvas::Private::RootTexture);
            }
        } else if (!loadPage) {
            // Scrolled beyond where this page is needed, skip it.
            if (page.requestId != 0) {
                d->document->cancelPageRequest(page.requestId);
                page.requestId = 0;
            }

            page.image = QImage();
            page.patches.clear();
            page.hasImage = false;
        }

        QSGTransformNode *t = static_cast<QSGTransformNode*>(root->childAtIndex(i));
        if (!t) {
            t = new QSGTransformNode;
            t->setFlag(QSGNode::OwnedByParent);
            root->appendChildNode(t);
        }

        QMatrix4x4 m;
        m.translate(0, page.rect.y());
        t->setMatrix(m);

        if (showPage) {
            QRectF inter = page.rect.intersected(visibleArea);
            qreal area = inter.width() * inter.height();
            // Select the current page as the page with the maximum
            // visible area.
            if (area > maxVisibleArea) {
                maxVisibleArea = area;
                currentPage = i + 1;
            }
        }

        if (loadPage) {
            // Node hierachy:
            // t
            // |-bg
            // |  |-tn
            // |  | |- patch1...
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

                if (!page.patches.empty()) {
                    QSGSimpleTextureNode *ptn = static_cast<QSGSimpleTextureNode*>(tn->firstChild());
                    for (QList<Patch>::iterator it = page.patches.begin();
                         it != page.patches.end(); it++) {
                        if (!ptn) {
                            ptn = new QSGSimpleTextureNode;
                            ptn->setFlag(QSGNode::OwnedByParent);
                            tn->appendChildNode(ptn);
                        }
                        
                        if (!it->second.isNull()) {
                            putTexture(window, ptn, it->second);
                            it->second = QImage();
                        }
                        setTextureRect(ptn, width(), page.renderWidth, it->first);

                        ptn = static_cast<QSGSimpleTextureNode*>(ptn->nextSibling());
                    }
                } else {
                    // Delete all previously registered patches.
                    for (QSGNode *child = tn->firstChild(); child; child = tn->firstChild())
                        delete child;
                }

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

    // prioritize in reverse order so we end up with a final priority list which is
    // ordered by increasing page number.
    for (int i = priorityRequests.size() - 1; i >= 0; --i) {
        const QPair<int, QPair<int, QRect> > &pr = priorityRequests.at(i);
        d->document->prioritizeRequest(pr.first, pr.second.first, pr.second.second);
    }

    if (d->currentPage != currentPage) {
        d->currentPage = currentPage;
        emit currentPageChanged();
    }

    return root;
}

void PDFCanvas::documentLoaded()
{
    d->pages.clear();
    d->pageCount = d->document->pageCount();
    d->renderWidth = width();
    layout();
}

void PDFCanvas::resizeTimeout()
{
    d->renderWidth = width();
    update();
}

void PDFCanvas::pageSizesFinished(const QList<QSizeF> &sizes)
{
    d->pageSizes = sizes;
    d->pageSizeRequested = false;
    layout();
}

QPair<int, QRectF> PDFCanvas::pageAtPoint(const QPointF &point) const
{
    for (int i = 0; i < d->pageCount; ++i) {
        const PDFPage& page = d->pages.value(i);
        if (page.rect.contains(point)) {
            return QPair<int, QRectF>{i, page.rect};
        }
    }
    return QPair<int, QRectF>{-1, QRectF()};
}
