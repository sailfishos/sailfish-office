/*
 *
 */

#include "pdfcanvas.h"

#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtQuick/QSGTransformNode>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleRectNode>

#include "pdfrenderthread.h"
#include "pdfdocument.h"

struct PDFPage {
    PDFPage()
        : index{-1}
        , requested{ false }
        , imageWidth{ -1 }
    { }

    int index;
    QRectF rect;

    bool requested;

    int imageWidth;
    QImage image;

    QList< QPair< QRectF, QUrl > > links;
};

class TextureNode : public QSGSimpleTextureNode
{
public:
    TextureNode() : imageKey(0), t(0) {}
    ~TextureNode() { delete t; }
    void setTexture(QSGTexture *texture) {
        t = texture;
        QSGSimpleTextureNode::setTexture(t);
    }

    qint64 imageKey;

private:
    QSGTexture *t;
};

class PDFCanvas::Private
{
public:
    Private( PDFCanvas* qq )
        : q{ qq }
        , pageCount{ 0 }
        , renderWidth{ 0 }
        , document{ nullptr }
        , flickable(0)
        , resizeTimer{ nullptr }
        , spacing{10.f}
    { }

    PDFCanvas* q;

    QHash< int, PDFPage > pages;

    int pageCount;

    int renderWidth;

    PDFDocument* document;
    QQuickItem *flickable;

    QTimer* resizeTimer;

    float spacing;

    QRectF visibleArea;

    QList< QSizeF > pageSizes;

    QColor linkColor;

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

        if( d->document->isLoaded() )
            documentLoaded();

        emit documentChanged();
    }
}

qreal PDFCanvas::pagePosition(int index) const
{
    if( d->pages.count() == 0 )
        return 0.f;

    return d->pages.value( index ).rect.y();
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

void PDFCanvas::layout()
{
    if(d->pageSizes.count() == 0)
    {
        d->document->requestPageSizes();
        return;
    }

    PDFDocument::LinkMap links = d->document->linkTargets();

    QSizeF firstPage = d->pageSizes.at(0);
    float scale = width() / firstPage.width();

    float totalHeight = 0.f;
    for( int i = 0; i < d->pageCount; ++i )
    {
        QSizeF unscaledSize = d->pageSizes.at( i );

        PDFPage page;
        page.index = i;
        page.rect = QRectF(0, totalHeight, unscaledSize.width() * scale, unscaledSize.height() * scale);
        page.image = d->pages.contains( i ) ? d->pages.value( i ).image : QImage();
        page.links = links.values( i );
        d->pages.insert( i, page );

        totalHeight += page.rect.height();
        if( i < d->pageCount - 1 )
            totalHeight += d->spacing;
    }

    setHeight(int(totalHeight));
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

void PDFCanvas::pageFinished( int id, const QImage& image )
{
    PDFPage& page = d->pages[ id ];

    page.image = image;
    page.imageWidth = image.width();
    page.requested = false;

    update();
}

void PDFCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QMetaObject::invokeMethod(d->resizeTimer, "start");
    layout();
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

QSGNode* PDFCanvas::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* )
{
    if( d->pageCount == 0 || !d->flickable )
    {
        if( node ) {
            delete node;
        }
        return nullptr;
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

    for( int i = 0; i < d->pageCount; ++i )
    {
        PDFPage& page = d->pages[ i ];

        if( page.rect.intersects( loadedArea ) )
        {
            if( page.image.isNull() && !page.requested )
            {
                d->document->requestPage( i, d->renderWidth );
                page.requested = true;
            }
        }
        else
        {
            if( !page.image.isNull() )
            {
                page.image = QImage();
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

        if( !page.image.isNull() && page.rect.intersects( visibleArea ) )
        {
            TextureNode* tn = static_cast< TextureNode* >( t->firstChild() );
            if(!tn)
            {
                tn = new TextureNode{};
                t->appendChildNode( tn );
            }

            if (tn->imageKey != page.image.cacheKey())
            {
                if (tn->texture())
                    delete tn->texture();
                tn->setTexture( window()->createTextureFromImage(page.image) );
                tn->imageKey = page.image.cacheKey();
            }
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
        else
        {
            if( t->childCount() > 0 )
            {
                delete t->firstChild();
                t->removeAllChildNodes();
            }
        }
    }

    return root;
}

void PDFCanvas::documentLoaded()
{
    d->pages.clear();
    d->pageCount = d->document->pageCount();

    d->renderWidth = width();
    d->document->requestPage( 0, d->renderWidth );

    layout();
}

void PDFCanvas::resizeTimeout()
{
    d->renderWidth = width();

    for( int i = 0; i < d->pageCount; ++i )
    {
        if( !d->pages[ i ].image.isNull() && d->pages[ i ].imageWidth != d->renderWidth )
            d->document->requestPage( i, d->renderWidth );
    }

    update();
}

void PDFCanvas::pageSizesFinished(const QList< QSizeF >& sizes)
{
    d->pageSizes = sizes;
    layout();
}
