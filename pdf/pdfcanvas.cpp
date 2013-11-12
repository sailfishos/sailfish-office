/*
 *
 */

#include "pdfcanvas.h"

#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtQuick/QSGTransformNode>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/QQuickWindow>

#include "pdfrenderthread.h"
#include "pdfdocument.h"

struct PDFPage {
    PDFPage() : index{-1}, requested{ false }
    { }

    int index;
    QRectF rect;
    QImage image;
    bool requested;
};

class PDFCanvas::Private
{
public:
    Private( PDFCanvas* qq )
        : q{ qq }
        , pageCount{ 0 }
        , document{ nullptr }
        , flickable(0)
        , resizeTimer{ nullptr }
        , spacing{10.f}
    { }

    PDFCanvas* q;

    QHash< int, PDFPage > pages;

    int pageCount;

    PDFDocument* document;
    QQuickItem *flickable;

    QTimer* resizeTimer;

    float spacing;

    QRectF visibleArea;

    QList< QSizeF > pageSizes;
};

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
        d->document->setCanvasWidth( width() );

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

void PDFCanvas::layout()
{
    if(d->pageSizes.count() == 0)
    {
        d->document->requestPageSizes();
        return;
    }

    QSizeF firstPage = d->pageSizes.at(0);
    float scale = width() / firstPage.width();

    float totalHeight = 0.f;
    for( int i = 0; i < d->pageCount; ++i )
    {
        QSizeF unscaledSize = d->pageSizes.at( i );

        PDFPage page;
        page.index = i;
        page.rect = QRectF(0, totalHeight, unscaledSize.width() * scale, unscaledSize.height() * scale);
        page.image = d->pages.contains( i ) ? d->pages.value( i ).image : QImage{};
        d->pages.insert( i, page );

        totalHeight += page.rect.height();
        if( i < d->pageCount - 1 )
            totalHeight += d->spacing;
    }

    setHeight(int(totalHeight));
    update();
}

void PDFCanvas::pageFinished( int id, const QImage& image )
{
    d->pages[ id ].image = image;
    d->pages[ id ].requested = false;

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

    //Loaded area equals visible area scaled to three times the size
    QRectF loadedArea = {
        visibleArea.x() - visibleArea.width(),
        visibleArea.y() - visibleArea.height(),
        visibleArea.width() * 3,
        visibleArea.height() * 3,
    };

    QSGTransformNode* root = static_cast<QSGTransformNode*>( node );
    if( !root ) {
        root = new QSGTransformNode{};
    }

    for( int i = 0; i < d->pageCount; ++i )
    {
        PDFPage& page = d->pages[ i ];

        if( page.rect.intersects( loadedArea ) )
        {
            if( page.image.isNull() && !page.requested )
            {
                d->document->requestPage( i, width() );
                page.requested = true;
            }
        }
        else
        {
            if( !page.image.isNull() )
                page.image = QImage{};
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
            QSGSimpleTextureNode* tn = static_cast< QSGSimpleTextureNode* >( t->firstChild() );
            if(!tn)
            {
                tn = new QSGSimpleTextureNode{};
                t->appendChildNode( tn );
            }

            tn->setRect( 0.f, 0.f, page.rect.size().width(), page.rect.size().height() );

            if( tn->texture() )
                delete tn->texture();

            tn->setTexture( window()->createTextureFromImage( page.image ) );
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

    d->document->setCanvasWidth( width() );
    d->document->requestPage( 0, width() );

    layout();
}

void PDFCanvas::resizeTimeout()
{
    d->document->setCanvasWidth( width() );

    for( int i = 0; i < d->pageCount; ++i )
    {
        if( !d->pages[ i ].image.isNull() )
            d->pages[ i ].image = QImage{};
    }
}

void PDFCanvas::pageSizesFinished(const QList< QSizeF >& sizes)
{
    d->pageSizes = sizes;
    layout();
}
