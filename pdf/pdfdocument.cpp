/*
 *
 */

#include "pdfdocument.h"
#include "pdfrenderthread.h"

#include <QDebug>
#include <QUrl>

#include <poppler-qt4.h>

class PDFDocument::Private
{
public:
    Private() : document( nullptr ), completed(false) { }

    PDFRenderThread* thread;

    Poppler::Document* document;
    QString source;
    bool completed;
};

PDFDocument::PDFDocument(QObject* parent)
    : QObject(parent), d(new Private())
{
    d->thread = new PDFRenderThread{ this };
    connect( d->thread, SIGNAL(loadFinished()), SIGNAL(documentLoaded()) );
    connect( d->thread, SIGNAL(loadFinished()), SIGNAL(pageCountChanged()) );
    connect( d->thread, SIGNAL(loadFinished()), SIGNAL(linkTargetsChanged()) );
    connect( d->thread, SIGNAL(pageFinished(int,QImage)), SIGNAL(pageFinished(int,QImage)));
}

PDFDocument::~PDFDocument()
{
}

QString PDFDocument::source() const
{
    return d->source;
}

int PDFDocument::pageCount() const
{
    return d->thread->pageCount();
}

QObject* PDFDocument::tocModel() const
{
    return d->thread->tocModel();
}

bool PDFDocument::isLoaded() const
{
    return d->thread->isLoaded();
}

QObjectList PDFDocument::linkTargets() const
{
    return d->thread->linkTargets();
}

void PDFDocument::classBegin()
{

}

void PDFDocument::componentComplete()
{
    if(!d->source.isEmpty())
    {
        d->thread->load( QUrl{ d->source }.toLocalFile() );
    }

    d->completed = true;
}

void PDFDocument::setSource(const QString& source)
{
    if (d->source != source)
    {
        d->source = source;

        if(d->completed) {
            d->thread->load( QUrl{ d->source }.toLocalFile() );
        }

        emit sourceChanged();
    }
}

void PDFDocument::setCanvasWidth(uint width)
{
    d->thread->setCanvasWidth( width );
}

void PDFDocument::requestPage(int index, int size)
{
    d->thread->requestPage( index, size );
}
