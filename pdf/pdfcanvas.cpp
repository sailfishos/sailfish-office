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

struct PDFPage {
    PDFPage()
        : index(-1)
        , requested(false)
        , renderWidth(0)
        , texture(nullptr)
    { }

    int index;
    QRectF rect;

    bool requested;

    int renderWidth;
    QRect textureArea;
    QSGTexture *texture;

    QList<QPair<QRectF, QUrl> > links;
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

    PDFCanvas *q;

    QHash<int, PDFPage> pages;
    bool pageSizeRequested;

    int pageCount;
    int currentPage;

    int renderWidth;

    PDFDocument *document;
    QQuickItem *flickable;

    QTimer *resizeTimer;

    float spacing;
    float linkWiggle;

    QRectF visibleArea;

    QList<QSizeF> pageSizes;

    QColor linkColor;
    QColor pagePlaceholderColor;

    QList<QSGTexture *> texturesToClean;
    QPointer<QQuickWindow> connectedWindow;

    void cleanPageTexturesLater(PDFPage &page)
    {
        if (page.texture) {
            texturesToClean << page.texture;
            page.texture = nullptr;
        }
    }

    void cleanTextures()
    {
        foreach (QSGTexture *texture, texturesToClean)
            delete texture;
        texturesToClean.clear();
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
    for (int i = 0; i < d->pageCount; ++i) {
        PDFPage &page = d->pages[i];
        page.texture->deleteLater();
        page.texture = nullptr;
    }

    delete d->resizeTimer;
    delete d;
}

QQuickItem * PDFCanvas::flickable() const
{
    return d->flickable;
}

void PDFCanvas::setFlickable(QQuickItem *f)
{
    if (f != d->flickable) {
        if (d->flickable)
            d->flickable->disconnect(this);

        d->flickable = f;
        connect(d->flickable, SIGNAL(contentXChanged()), this, SLOT(update()));
        connect(d->flickable, SIGNAL(contentYChanged()), this, SLOT(update()));
        connect(d->flickable, SIGNAL(widthChanged()), this, SLOT(update()));

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

        connect(d->document, &PDFDocument::documentLoadedChanged, this, &PDFCanvas::documentLoaded);
        connect(d->document, &PDFDocument::pageFinished, this, &PDFCanvas::pageFinished);
        connect(d->document, &PDFDocument::pageSizesFinished, this, &PDFCanvas::pageSizesFinished);
        connect(d->document, &PDFDocument::documentLockedChanged, this, &PDFCanvas::documentLoaded);

        if (d->document->isLoaded())
            documentLoaded();

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
        if (d->document->isLoaded() && d->pageCount > 0 && !d->pageSizeRequested) {
            d->document->requestPageSizes();
            d->pageSizeRequested = true;
        }
        return;
    }

    PDFDocument::LinkMap links = d->document->linkTargets();

    float totalHeight = 0.f;
    for (int i = 0; i < d->pageCount; ++i) {
        QSizeF unscaledSize = d->pageSizes.at(i);
        float ratio = unscaledSize.height() / unscaledSize.width();

        PDFPage page;
        page.index = i;
        page.rect = QRectF(0, totalHeight, width(), width() * ratio);
        page.links = links.values(i);
        page.requested = false; // We're cancelling all requests below
        if (d->pages.contains(i)) {
            page.renderWidth = d->pages.value(i).renderWidth;
            page.textureArea = d->pages.value(i).textureArea;
            page.texture = d->pages.value(i).texture;
        }
        d->pages.insert(i, page);

        totalHeight += page.rect.height();
        if (i < d->pageCount - 1)
            totalHeight += d->spacing;
    }

    setHeight(int(totalHeight));

    // We're going to be requesting new images for all content, so remove
    // pending reuqests to minimize the delay before they come.
    d->document->cancelPageRequest(-1);

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

void PDFCanvas::pageFinished(int id, int pageRenderWidth,
                             QRect subpart, QSGTexture *texture)
{
    PDFPage &page = d->pages[id];

    d->cleanPageTexturesLater(page);

    page.renderWidth = pageRenderWidth;
    page.textureArea = subpart;
    page.texture = texture;
    page.requested = false;

    update();
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
    d->document->cancelPageRequest(-1);
    d->cleanTextures();
    for (int i = 0; i < d->pageCount; ++i) {
        PDFPage &page = d->pages[i];
        delete page.texture;
        page.texture = nullptr;
        page.requested = false;
    }
}

QSGNode* PDFCanvas::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *)
{
    if (d->pageCount == 0 || !d->flickable) {
        delete node;
        d->cleanTextures();
        return nullptr;
    }

    if (window() != d->connectedWindow) {
        d->connectedWindow = window();
        connect(window(), &QQuickWindow::sceneGraphInvalidated, this, &PDFCanvas::sceneGraphInvalidated, Qt::DirectConnection);
    }

    //Visible area equals flickable translated by contentX/Y
    QRectF visibleArea{ d->flickable->property("contentX").toFloat(),
                d->flickable->property("contentY").toFloat(),
                d->flickable->width(), d->flickable->height() };

    //Loaded area equals visible area scaled to five times the size
    QRectF loadedArea = {
        visibleArea.x() - visibleArea.width() * 2,
        visibleArea.y() - visibleArea.height() * 2,
        visibleArea.width() * 5,
        visibleArea.height() * 5,
    };
    QRect textureLimit = {
        0, 0,
        int(2.5 * qMin(window()->width(), window()->height())),
        int(2.5 * qMin(window()->width(), window()->height()))
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
            textureLimit.moveTo(0, 0);
            bool fullPageFit = textureLimit.contains(pageRect);
            QRect showableArea = {
                int(renderingRatio * (visibleArea.x() - float(window()->width() / 4.) - page.rect.x())),
                int(renderingRatio * (visibleArea.y() - float(window()->height() / 4.) - page.rect.y())),
                int(renderingRatio * (visibleArea.width() + float(window()->width() / 2.))),
                int(renderingRatio * (visibleArea.height() + float(window()->height() / 2.)))
            };
            showableArea = showableArea.intersected(pageRect);
            // Limit showableArea with textureLimit to avoid looping request
            // for something too big.
            textureLimit.moveCenter(showableArea.center());
            showableArea = showableArea.intersected(textureLimit);

            if (page.texture == nullptr
                || page.renderWidth != d->renderWidth
                || (page.renderWidth == int(width()) &&
                    !page.textureArea.contains(showableArea))) {
                QRect request = (fullPageFit) ? QRect() : textureLimit;
                if (!page.requested) {
                    d->document->requestPage(i, d->renderWidth, window(), request);
                    page.requested = true;
                }
                priorityRequests << QPair<int, QPair<int, QRect> >(i, QPair<int, QRect>(d->renderWidth, request));
            }
        } else if (loadPage
                   && !page.requested && (page.renderWidth != d->renderWidth)) {
            textureLimit.moveTo(0, 0);
            // We preload full page only if they can fit into texture.
            if (textureLimit.contains(pageRect)) {
                d->document->requestPage(i, d->renderWidth, window());
                page.requested = true;
            }
        } else if (!loadPage) {
            d->cleanPageTexturesLater(page);

            // Scrolled beyond where this page is needed, skip it.
            if (page.requested) {
                d->document->cancelPageRequest(i);
                page.requested = false;
            }
        }

        QSGTransformNode *t = static_cast<QSGTransformNode*>(root->childAtIndex(i));
        if (!t) {
            t = new QSGTransformNode;
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

        if (showPage) {
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
                bg->setColor(d->pagePlaceholderColor);
                t->appendChildNode(bg);
            }
            bg->setRect(0., 0., page.rect.width(), page.rect.height());

            if (page.texture) {
                QSGSimpleTextureNode *tn = static_cast<QSGSimpleTextureNode *>(bg->firstChild());
                if (!tn) {
                    tn = new QSGSimpleTextureNode;
                    bg->appendChildNode(tn);
                }
                tn->setTexture(page.texture);
                if (int(width()) == page.renderWidth) {
                    tn->setRect(page.textureArea);
                } else {
                    float ratio = width() / page.renderWidth;
                    tn->setRect(int(ratio * page.textureArea.x()),
                                int(ratio * page.textureArea.y()),
                                qCeil(ratio * page.textureArea.width()),
                                qCeil(ratio * page.textureArea.height()));
                }

                QSGNode *n = tn->nextSibling();
                if (!n) {
                    n = new QSGNode;
                    bg->appendChildNode(n);
                }
                QSGSimpleRectNode *rn = static_cast<QSGSimpleRectNode*>(n->firstChild());
                for (int l = 0; l < page.links.count(); ++l) {
                    if (!rn) {
                        rn = new QSGSimpleRectNode;
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
            }
        } else {
            delete t->firstChild();
            t->removeAllChildNodes();
        }
    }

    // prioritize in reverse order so we end up with a final priority list which is
    // ordered by increasing page number.
    for (int i = priorityRequests.size() - 1; i >= 0; --i) {
        const QPair<int, QPair<int, QRect> > &pr = priorityRequests.at(i);
        d->document->prioritizeRequest(pr.first, pr.second.first, pr.second.second);
    }

    d->cleanTextures();

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
