/*
 *
 */

#include "pdfcanvas.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "pdfrenderthread.h"
#include "pdfdocument.h"

class PDFCanvas::Private
{
public:
    Private( PDFCanvas* qq ) : q{ qq }, pageCount{ 0 }, document{ nullptr } { }

    QImage bestMatchingImage( int index );

    PDFCanvas* q;

    QHash< int, QHash< uint, QImage > > images;

    int pageCount;

    PDFDocument* document;
};

PDFCanvas::PDFCanvas(QDeclarativeItem* parent)
    : QDeclarativeItem(parent), d(new Private(this))
{
    setFlag( QGraphicsItem::ItemHasNoContents, false );
    setFlag( QGraphicsItem::ItemSendsGeometryChanges, true );
}

PDFCanvas::~PDFCanvas()
{
    delete d;
}

void PDFCanvas::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* )
{
    if( d->pageCount == 0 )
        return;

    QImage firstImage = d->bestMatchingImage( 0 );
    if( firstImage.isNull() )
        return;

    int totalHeight = 0;
    int pageHeight = firstImage.height() * ( width() / firstImage.width() );

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

    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
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
