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
        : index{-1}
        , requested{ false }
        , rectPlaceholder{ false }
        , texture{ nullptr }
    { }

    int index;
    QRectF rect;
    bool requested;
    bool rectPlaceholder;
    QSGTexture *texture;

    QList< QPair< QRectF, QUrl > > links;
};

class PDFCanvas::Private
{
public:
    Private( PDFCanvas* qq )
        : q{ qq }
        , pageCount{ 0 }
        , currentPage{ 1 }
        , renderWidth{ 0 }
        , document{ nullptr }
        , flickable(0)
        , resizeTimer{ nullptr }
        , spacing{10.f}
    { }

    PDFCanvas* q;

    QHash< int, PDFPage > pages;

    int pageCount;
    int currentPage;

    int renderWidth;

    PDFDocument* document;
    QQuickItem *flickable;

    QTimer* resizeTimer;

    float spacing;

    QRectF visibleArea;

    QList< QSizeF > pageSizes;

    QColor linkColor;
    QColor pagePlaceholderColor;

    QList<QSGTexture *> texturesToClean;
    QPointer<QQuickWindow> connectedWindow;

    void cleanTextures()
    {
        foreach (QSGTexture *texture, texturesToClean)
            delete texture;
        texturesToClean.clear();
    }

    static const float wiggleFactor;
};


const float PDFCanvas::Private::wiggleFactor{ 4.f };

PDFCanvas::PDFCanvas(QQuickItem* parent)
    : QQuickItem(parent), d(new Private(this))
{
    setFlag(ItemHasContents, true);

    d->resizeTimer = new QTimer{};
    d->resizeTimer->setInterval(500);
    d->resizeTimer->setSingleShot(true);
    connect(d->resizeTimer, &QTimer::timeout, this, &PDFCanvas::resizeTimeout);
}

PDFCanvas::~PDFCanvas()
{
    if(d->resizeTimer)
        delete d->resizeTimer;

    delete d;
}

QQuickItem * PDFCanvas::flickable() const
{
    return d->flickable;
}

void PDFCanvas::setFlickable(QQuickItem *f)
{
    if( f != d->flickable )
    {
        if( d->flickable )
            d->flickable->disconnect(this);

        d->flickable = f;
        connect(d->flickable, SIGNAL(contentXChanged()), this, SLOT(update()));
        connect(d->flickable, SIGNAL(contentYChanged()), this, SLOT(update()));

        emit flickableChanged();
    }
}

PDFDocument* PDFCanvas::document() const
{
    return d->document;
}

void PDFCanvas::setDocument(PDFDocument* doc)
{
    if(doc != d->document) {
        if( d->document )
        {
            d->document->disconnect(this);
            d->pageSizes.clear();
        }

        d->document = doc;

        connect( d->document, &PDFDocument::documentLoaded, this, &PDFCanvas::documentLoaded );
        connect( d->document, &PDFDocument::pageFinished, this, &PDFCanvas::pageFinished );
        connect( d->document, &PDFDocument::pageSizesFinished, this, &PDFCanvas::pageSizesFinished );
        connect( d->document, &PDFDocument::documentLocked, this, &PDFCanvas::documentLoaded );

        if( d->document->isLoaded() )
            documentLoaded();

        emit documentChanged();
    }
}

QRectF PDFCanvas::pageRectangle(int index) const
{
    if( d->pages.count() == 0 )
        return QRectF();

    return d->pages.value( index ).rect;
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
    if(newValue != d->spacing) {
        d->spacing = newValue;
        emit spacingChanged();
    }
}

QColor PDFCanvas::linkColor() const
{
    return d->linkColor;
}

void PDFCanvas::setLinkColor(const QColor& color)
{
    if( color != d->linkColor )
    {
        d->linkColor = color;
        d->linkColor.setAlphaF( 0.25 );
        update();
        emit linkColorChanged();
    }
}
QColor PDFCanvas::pagePlaceholderColor() const
{
    return d->pagePlaceholderColor;
}

void PDFCanvas::setPagePlaceholderColor(const QColor& color)
{
    if( color != d->pagePlaceholderColor )
    {
        d->pagePlaceholderColor = color;
        d->pagePlaceholderColor.setAlphaF( 0.25 );
        update();
        emit pagePlaceholderColorChanged();
    }
}

void PDFCanvas::layout()
{
    if(d->pageSizes.count() == 0)
    {
        d->document->requestPageSizes();
        return;
    }

    PDFDocument::LinkMap links = d->document->linkTargets();

    float totalHeight = 0.f;
    for( int i = 0; i < d->pageCount; ++i )
    {
        QSizeF unscaledSize = d->pageSizes.at( i );
        float ratio = unscaledSize.height() / unscaledSize.width();

        PDFPage page;
        page.index = i;
        page.rect = QRectF(0, totalHeight, width(), width() * ratio);
        page.links = links.values( i );
        page.requested = false; // We're cancelling all requests below
        if (d->pages.contains(i)) {
            page.texture = d->pages.value( i ).texture;
        }
        d->pages.insert( i, page );

        totalHeight += page.rect.height();
        if( i < d->pageCount - 1 )
            totalHeight += d->spacing;
    }

    setHeight(int(totalHeight));

    // We're going to be requesting new images for all content, so remove
    // pending reuqests to minimize the delay before they come.
    d->document->cancelPageRequest(-1);

    emit pageLayoutChanged();

    update();
}

QUrl PDFCanvas::urlAtPoint(const QPoint& point)
{
    for( int i = 0; i < d->pageCount; ++i )
    {
        const PDFPage& page = d->pages.value( i );
        if( page.rect.contains( point ) )
        {
            for( const QPair< QRectF, QUrl >& link : page.links )
            {
                QRectF hitTarget{
                    link.first.x() * page.rect.width() - Private::wiggleFactor,
                    link.first.y() * page.rect.height() - Private::wiggleFactor + page.rect.y(),
                    link.first.width() * page.rect.width() + Private::wiggleFactor * 2,
                    link.first.height() * page.rect.height() + Private::wiggleFactor * 2
                };

                if( hitTarget.contains( point ) )
                    return link.second;
            }
        }
    }

    return QUrl();
}

QRectF PDFCanvas::fromPageToItem(int index, const QRectF &rect)
{
    if (index < 0 || index >= d->pageCount)
        return QRectF();

    const PDFPage &page = d->pages.value(index);
    return QRectF(rect.x() * page.rect.width() + page.rect.x(),
                  rect.y() * page.rect.height() + page.rect.y(),
                  rect.width() * page.rect.width(),
                  rect.height() * page.rect.height());
}

void PDFCanvas::pageFinished( int id, QSGTexture *texture )
{
    PDFPage& page = d->pages[ id ];

    if (page.texture)
        d->texturesToClean << page.texture;
    page.texture = texture;
    page.requested = false;

    update();
}

void PDFCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
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
    for( int i = 0; i < d->pageCount; ++i )
    {
        PDFPage& page = d->pages[ i ];
        if (page.texture) {
            delete page.texture;
            page.texture = 0;
        }
        page.requested = false;
        page.rectPlaceholder = false;
    }
}

QSGNode* PDFCanvas::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData *)
{
    if( d->pageCount == 0 || !d->flickable )
    {
        if( node ) {
            delete node;
        }
        d->cleanTextures();
        return nullptr;
    }

    if (window() != d->connectedWindow) {
        d->connectedWindow = window();
        connect(window(), &QQuickWindow::sceneGraphInvalidated, this, &PDFCanvas::sceneGraphInvalidated, Qt::DirectConnection);
    }

    //Visible area equals flickable translated by contentX/Y
    QRectF visibleArea{ d->flickable->property("contentX").toFloat(), d->flickable->property("contentY").toFloat(), d->flickable->width(), d->flickable->height() };

    //Loaded area equals visible area scaled to five     times the size
    QRectF loadedArea = {
        visibleArea.x() - visibleArea.width() * 2,
        visibleArea.y() - visibleArea.height() * 2,
        visibleArea.width() * 5,
        visibleArea.height() * 5,
    };

    QSGNode* root = static_cast<QSGNode*>( node );
    if( !root ) {
        root = new QSGNode{};
    }

    QList<QPair<int, int> > priorityRequests;
    int currentPage = d->currentPage;
    qreal maxVisibleArea = 0.;

    for( int i = 0; i < d->pageCount; ++i )
    {
        PDFPage& page = d->pages[ i ];

        bool loadPage = page.rect.intersects(loadedArea);
        bool showPage = page.rect.intersects(visibleArea);

        if( loadPage )
        {
            if ((!page.texture)
                || (page.texture && page.texture->textureSize().width() != d->renderWidth))
            {
                if (page.requested && showPage) {
                    priorityRequests << QPair<int, int>(i, d->renderWidth);
                } else if (!page.requested) {
                    d->document->requestPage( i, d->renderWidth, window() );
                    page.requested = true;
                }
            }
        }
        else
        {
            if( page.texture )
            {
                d->texturesToClean << page.texture;
                page.texture = 0;
            }

            // Scrolled beyond where this page is needed, skip it.
            if (page.requested)
            {
                d->document->cancelPageRequest(i);
                page.requested = false;
            }
        }

        QSGTransformNode* t = static_cast<QSGTransformNode*>( root->childAtIndex(i) );
        if(!t)
        {
            t = new QSGTransformNode{};
            root->appendChildNode(t);
        }

        QMatrix4x4 m;
        m.translate( 0, page.rect.y() );
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

        if( page.texture && showPage )
        {
            if ( page.rectPlaceholder ) {
                delete t->firstChild();
                t->removeAllChildNodes();
            }
            QSGSimpleTextureNode *tn = static_cast<QSGSimpleTextureNode *>(t->firstChild());
            if (!tn)
            {
                tn = new QSGSimpleTextureNode{};
                t->appendChildNode( tn );
            }

            page.rectPlaceholder = false;
            tn->setTexture(page.texture);
            tn->setRect( 0.f, 0.f, page.rect.size().width(), page.rect.size().height() );

            if( page.links.count() > 0 )
            {
                for( int l = 0; l < page.links.count(); ++l )
                {
                    QRectF linkRect = page.links.value( l ).first;

                    QSGSimpleRectNode* linkNode = static_cast< QSGSimpleRectNode* >( tn->childAtIndex( l ) );
                    if( !linkNode )
                    {

                        linkNode = new QSGSimpleRectNode;
                        tn->appendChildNode( linkNode );
                    }

                    QRectF targetRect{
                        linkRect.x() * page.rect.width(),
                        linkRect.y() * page.rect.height(),
                        linkRect.width() * page.rect.width(),
                        linkRect.height() * page.rect.height()
                    };

                    linkNode->setRect( targetRect );
                    linkNode->setColor( d->linkColor );
                }
            }
        }
        else if ( !page.rectPlaceholder && showPage )
        {
            QSGSimpleRectNode *bgNode = new QSGSimpleRectNode();
            t->appendChildNode( bgNode );

            page.rectPlaceholder = true;
            bgNode->setRect( 0., 0., page.rect.width(), page.rect.height() );
            bgNode->setColor( d->pagePlaceholderColor );
        }
        else if ( !showPage )
        {
            page.rectPlaceholder = false;
            if( t->childCount() > 0 ) {
                delete t->firstChild();
                t->removeAllChildNodes();
            }
        }
    }

    // prioritize in reverse order so we end up with a final priority list which is
    // ordered by increasing page number.
    for (int i=priorityRequests.size() - 1; i >= 0; --i) {
        const QPair<int, int> &pr = priorityRequests.at(i);
        d->document->prioritizeRequest(pr.first, pr.second);
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

void PDFCanvas::pageSizesFinished(const QList< QSizeF >& sizes)
{
    d->pageSizes = sizes;
    layout();
}
