/*
 *
 */

#include "pdfdocument.h"
#include "pdfrenderthread.h"
#include "pdftocmodel.h"

#include <QDebug>
#include <QUrl>

#include <poppler-qt4.h>

class PDFDocument::Private
{
public:
    Private() : document( nullptr ), completed(false) { }

    Poppler::Document* document;
    //PDFTocModel* tocModel;
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

QObject* PDFDocument::tocModel() const
{
    return 0;//d->tocModel;
}

void PDFDocument::classBegin()
{

}

void PDFDocument::componentComplete()
{
    if(!d->source.isEmpty())
    {
        PDFRenderThread::instance()->load( QUrl{ d->source }.toLocalFile() );
        //if(d->tocModel)
            //d->tocModel->deleteLater();
        //d->tocModel = new PDFTocModel(d->document, this);
        emit tocModelChanged();
        emit pageCountChanged();
    }

    d->completed = true;
}

void PDFDocument::setSource(const QString& source)
{
    if (d->source != source)
    {
        d->source = source;
        //if(d->tocModel)
            //d->tocModel->deleteLater();

        if(d->completed) {
            PDFRenderThread::instance()->load( QUrl{ d->source }.toLocalFile() );
            //d->tocModel = new PDFTocModel(d->document, this);
        }

        emit tocModelChanged();
        emit sourceChanged();
        emit pageCountChanged();
    }
}
