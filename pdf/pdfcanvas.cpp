/*
 *
 */

#include "pdfcanvas.h"
#include "pdfrenderthread.h"
#include <QPainter>

class PDFCanvas::Private
{
public:
    Private( PDFCanvas* qq ) : q{ qq }, pageCount{ 0 } { }

    QImage bestMatchingImage( int index );

    PDFCanvas* q;

    QHash< int, QHash< uint, QImage > > images;

    int pageCount;

};

PDFCanvas::PDFCanvas(QDeclarativeItem* parent)
    : QDeclarativeItem(parent), d(new Private(this))
{
    setFlag( QGraphicsItem::ItemHasNoContents, false );
    setFlag( QGraphicsItem::ItemSendsGeometryChanges, true );

    connect(PDFRenderThread::instance(), SIGNAL(loadFinished()), SLOT(documentLoaded()));
    if(PDFRenderThread::instance()->isLoaded())
        documentLoaded();

    connect(PDFRenderThread::instance(), SIGNAL(pageFinished(int,QImage)), SLOT(pageFinished(int,QImage)));
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
        if( img.width() != width() )
            PDFRenderThread::instance()->requestPage( i, width() );

        painter->drawImage( QRect( 0, totalHeight, width(), pageHeight ), img, img.rect() );

        if( i < d->pageCount - 1)
            totalHeight += pageHeight + pageHeight * 0.025;
        else
            totalHeight += pageHeight;
    }

    if( height() != totalHeight )
        setHeight( totalHeight );
}

qreal PDFCanvas::pagePosition(int index) const
{
    QImage firstImage = d->bestMatchingImage( 0 );
    if( firstImage.isNull() )
        return 0.f;

    int pageHeight = firstImage.height() * ( width() / firstImage.width() );
    int spacing = pageHeight * 0.025;

    return pageHeight * index + ( qMax( index - 1, 0 ) * spacing );
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
    d->pageCount = PDFRenderThread::instance()->pageCount();

    for( int i = 0; i < d->pageCount; ++i )
        PDFRenderThread::instance()->requestPage( i, width() );
}
