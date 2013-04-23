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

    Poppler::Document* document;
    QString source;
    bool completed;
};

PDFDocument::PDFDocument(QObject* parent)
    : QObject(parent), d(new Private())
{

}

PDFDocument::~PDFDocument()
{
    delete d->document;
    delete d;
}

QString PDFDocument::source() const
{
    return d->source;
}

int PDFDocument::pageCount() const
{
    return PDFRenderThread::instance()->pageCount();
}

void PDFDocument::classBegin()
{

}

void PDFDocument::componentComplete()
{
    if(!d->source.isEmpty())
    {
        PDFRenderThread::instance()->load( QUrl{ d->source }.toLocalFile() );
        emit pageCountChanged();
    }

    d->completed = true;
}

void PDFDocument::setSource(const QString& source)
{
    if (d->source != source)
    {
        d->source = source;

        if(d->completed) {
            PDFRenderThread::instance()->load( QUrl{ d->source }.toLocalFile() );
        }

        emit sourceChanged();
        emit pageCountChanged();
    }
}
