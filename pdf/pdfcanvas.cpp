/*
 *
 */

#include "pdfcanvas.h"

#include <QtGui/QPainter>
#include <QtQuick/QSGTransformNode>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/QQuickWindow>

#include "pdfrenderthread.h"
#include "pdfdocument.h"

class PDFCanvas::Private
{
public:
    Private( PDFCanvas* qq ) : q{ qq }, pageCount{ 0 }, document{ nullptr }, flickable(0){ }

    QImage bestMatchingImage( int index );

    PDFCanvas* q;

    QHash< int, QHash< uint, QImage > > images;

    int pageCount;

    PDFDocument* document;
    QQuickItem *flickable;
};

PDFCanvas::PDFCanvas(QQuickItem* parent)
    : QQuickItem(parent), d(new Private(this))
{
    setFlag(ItemHasContents, true);
    // FIXME port needed ? setFlag( QGraphicsItem::ItemHasNoContents, false );
}

PDFCanvas::~PDFCanvas()
{
    delete d;
}

/*void PDFCanvas::paint( QPainter* painter)
{
    if( d->pageCount == 0 )
        return;

    QImage firstImage = d->bestMatchingImage( 0 );
    if( firstImage.isNull() )
        return;

    int totalHeight = 0;
    int pageHeight = firstImage.height() * ( width() / firstImage.width() );


    painter->setClipRect(QRectF(d->flickable->property("contentX").toDouble(), d->flickable->property("contentY").toDouble(), d->flickable->width(), d->flickable->height()));

    for( int i = 0; i < d->pageCount; ++i )
    {
        QImage img = d->bestMatchingImage( i );
        if( img.width() != int(width()) )
        {
            d->document->requestPage( i, width() );
            if( img.isNull() )
                pageFinished( i, QImage( width(), pageHeight, QImage::Format_ARGB32 ) );
            else
                pageFinished( i, img.scaled( width(), pageHeight ) );
        }

        QRect targetRect = QRect( 0, totalHeight, width(), pageHeight );
        if( targetRect.intersects( painter->clipBoundingRect().toAlignedRect() ) )
        {
            painter->drawImage( targetRect, img, img.rect() );
        }

        if( i < d->pageCount - 1)
            totalHeight += pageHeight + pageHeight * 0.025;
        else
            totalHeight += pageHeight;
    }
    if( int(height()) != totalHeight )
        setHeight( totalHeight );
}*/

QQuickItem * PDFCanvas::flickable() const
{
    return d->flickable;
}

void PDFCanvas::setFlickable(QQuickItem *f)
{
    d->flickable = f;
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
            disconnect(this, SLOT(documentLoaded()));
            disconnect(this, SLOT(pageFinished(int,QImage)));
        }

        d->document = doc;
        d->document->setCanvasWidth( width() );

        connect(d->document, SIGNAL(documentLoaded()), SLOT(documentLoaded()));
        connect(d->document, SIGNAL(pageFinished(int,QImage)), SLOT(pageFinished(int,QImage)));
        if( d->document->isLoaded() )
            documentLoaded();

        emit documentChanged();
    }
}

qreal PDFCanvas::pagePosition(int index) const
{
    QImage firstImage = d->bestMatchingImage( 0 );
    if( firstImage.isNull() )
        return 0.f;

    int pageHeight = firstImage.height() * ( width() / firstImage.width() );
    int spacing = pageHeight * 0.025;

    return pageHeight * ( index - 1 ) + ( qMax( index - 2, 0 ) * spacing );
}

void PDFCanvas::pageFinished( int id, const QImage& image )
{
    if( d->images.contains( id ) )
    {
        d->images[ id ].insert( image.width(), image );
    }
    else
    {
        QHash< uint, QImage > sizes;
        sizes.insert( image.width(), image );
        d->images.insert( id, sizes );
    }

    update();
}

void PDFCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if( d->document )
        d->document->setCanvasWidth( newGeometry.width() );

    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

QSGNode* PDFCanvas::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* )
{
    if( d->pageCount == 0 || !d->flickable )
    {
        if(node) {
            delete node;
        }
        return nullptr;
    }

    QImage firstImage = d->bestMatchingImage( 0 );
    if( firstImage.isNull() )
        return nullptr;

    int totalHeight = 0;
    int pageHeight = firstImage.height() * ( width() / firstImage.width() );

    QRectF frustum{ d->flickable->property("contentX").toFloat(), d->flickable->property("contentY").toFloat(), d->flickable->width(), d->flickable->height() };

    QSGTransformNode* root = static_cast<QSGTransformNode*>(node);
    if(!root) {
        root = new QSGTransformNode{};
    }

    for( int i = 0; i < d->pageCount; ++i )
    {
        QImage img = d->bestMatchingImage( i );
        if( img.width() != int(width()) )
        {
            d->document->requestPage( i, width() );
            if( img.isNull() )
                pageFinished( i, QImage( width(), pageHeight, QImage::Format_ARGB32 ) );
            else
                pageFinished( i, img.scaled( width(), pageHeight ) );
        }

        QSGTransformNode* t = static_cast<QSGTransformNode*>(root->childAtIndex(i));
        if(!t) {
            t = new QSGTransformNode{};
            root->appendChildNode(t);
        }

        QMatrix4x4 m;
        m.translate(0, totalHeight);
        t->setMatrix(m);

        QRectF targetRect = QRect( 0, totalHeight, width(), pageHeight );
        if( targetRect.intersects( frustum ) )
        {
            if(!t->childCount() > 0)
            {
                QSGSimpleTextureNode* tn = new QSGSimpleTextureNode{};
                t->appendChildNode(tn);
                tn->setRect(img.rect());
                tn->setTexture(window()->createTextureFromImage(img));
            }
        }
        else
        {
            if(t->childCount() > 0) {
                delete t->firstChild();
                t->removeAllChildNodes();
            }
        }

        if( i < d->pageCount - 1)
            totalHeight += pageHeight + pageHeight * 0.025;
        else
            totalHeight += pageHeight;
    }
    if( int(height()) != totalHeight )
        setHeight( totalHeight );

    return root;
}

QImage PDFCanvas::Private::bestMatchingImage(int index)
{
    auto sizes = images.value( index );
    if( sizes.contains( q->width() ) )
        return sizes.value( q->width() );

    int target = -1;
    foreach( uint width, sizes.keys() )
    {
        if( target == -1 || qAbs( width - q->width() ) < qAbs( target - q->width() ) )
            target = width;
    }

    if( target != -1 )
        return sizes.value( uint( target ) );

    return QImage();
}

void PDFCanvas::documentLoaded()
{
    d->images.clear();
    d->pageCount = d->document->pageCount();

    d->document->setCanvasWidth( width() );
    d->document->requestPage( 0, width() );
}
