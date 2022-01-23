/* -*- c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2016 Caliste Damien.
 * Copyright (C) 2022 Yura Beznos <yura.beznos@you-ra.info>
 * Contact: Damien Caliste <dcaliste@free.fr>
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

#include "djvuannotation.h"

static QString currentAuthor;

class DJVUAnnotation::Private
{
public:
    Private(Poppler::Annotation *annotation, DJVUDocument *document = nullptr, int page = -1)
        : owner(document == nullptr || page < 0)
        , m_annotation(annotation)
        , m_document(document)
        , m_page(page)
    {
    }
    ~Private()
    {
        if (owner)
            delete m_annotation;
    }
    bool owner;
    //Poppler::Annotation *m_annotation;
    KDjVu::Annotation *m_annotation;
    DJVUDocument *m_document;
    int m_page;
};

DJVUAnnotation::DJVUAnnotation(KDjVu::Annotation /*Poppler::Annotation*/ *annotation, QObject *parent)
    : QObject(parent), d(new DJVUAnnotation::Private(annotation))
{
    if (annotation && !currentAuthor.isEmpty())
        annotation->setAuthor(currentAuthor);
}

DJVUAnnotation::DJVUAnnotation(KDjVu::Annotation /*Poppler::Annotation*/ *annotation,
                             DJVUDocument *document, int ipage, QObject *parent)
    : QObject(parent), d(new DJVUAnnotation::Private(annotation, document, ipage))
{
}

DJVUAnnotation::~DJVUAnnotation()
{
    delete d;
}

DJVUDocument* DJVUAnnotation::document() const
{
    return d->m_document;
}

int DJVUAnnotation::page() const
{
    return d->m_page;
}

QRectF DJVUAnnotation::boundary() const
{
    return d->m_annotation->boundary();
}

void DJVUAnnotation::attachOnce(DJVUDocument *document, int page)
{
    /* Attach annotation only once. */
    if (d->m_document != nullptr || document == nullptr)
        return;
    if (d->m_page >= 0 || page < 0 || page >= document->pageCount())
        return;

    d->m_document = document;
    d->m_page = page;
}

void DJVUAnnotation::attach(DJVUDocument *document, DJVUSelection *selection)
{
    if (!selection || selection->count() == 0)
        return;

    QPair<int, QRectF> word = selection->rectAt(0);
    attachOnce(document, word.first);

    QRectF bounds = word.second;
    for (int i = 1; i < selection->count(); i++)
        bounds |= selection->rectAt(i).second;

    d->m_annotation->setBoundary(bounds);
    d->m_document->addAnnotation(d->m_annotation, d->m_page);
    emit attached();
}

void DJVUAnnotation::remove()
{
    if (d->m_document && d->m_annotation) {
        d->m_document->removeAnnotation(d->m_annotation, d->m_page);
        d->m_annotation = nullptr;
    }

    deleteLater();
}

QString DJVUAnnotation::author() const
{
    return d->m_annotation->author();
}

void DJVUAnnotation::setAuthor(const QString &value)
{
    // Store last given author to use it as default for
    // newly created annotations.
    currentAuthor = value;

    if (d->m_annotation->author() == value)
        return;
    d->m_annotation->setAuthor(value);
    emit authorChanged();

    d->m_annotation->setModificationDate(QDateTime());
    emit modificationDateChanged();

    if (d->m_document != nullptr)
        d->m_document->setDocumentModified();
}

QString DJVUAnnotation::contents() const
{
    return d->m_annotation->contents();
}

void DJVUAnnotation::setContents(const QString &value)
{
    if (d->m_annotation->contents() == value)
        return;

    d->m_annotation->setContents(value);
    emit contentsChanged();

    d->m_annotation->setModificationDate(QDateTime());
    emit modificationDateChanged();

    if (d->m_document != nullptr)
        d->m_document->setDocumentModified();
}

QDateTime DJVUAnnotation::creationDate() const
{
    return d->m_annotation->creationDate();
}

QDateTime DJVUAnnotation::modificationDate() const
{
    return d->m_annotation->modificationDate();
}

QColor DJVUAnnotation::color() const
{
    return d->m_annotation->style().color();
}

void DJVUAnnotation::setColor(const QColor &value)
{
    if (color() == value)
        return;

    //Poppler::Annotation::Style style = d->m_annotation->style();

    //style.setColor(value);
    //d->m_annotation->setStyle(style);
    //emit colorChanged();

    if (d->m_document != nullptr && d->m_page >= 0)
        d->m_document->onPageModified(d->m_page, d->m_annotation->boundary());
}

DJVUAnnotation::SubType DJVUAnnotation::type() const
{
    return DJVUAnnotation::SubType(d->m_annotation->subType());
}


static QStringList iconNames = (QStringList() << "Note" << "Comment" << "Key" << "Help" << "NewParagraph" << "Paragraph" << "Insert" << "Cross" << "Circle");

//DJVUTextAnnotation::DJVUTextAnnotation(QObject *parent)
    : DJVUAnnotation(new Poppler::TextAnnotation(Poppler::TextAnnotation::Linked), parent)
//{
//}

DJVUTextAnnotation::DJVUTextAnnotation(Poppler::TextAnnotation *annotation,
                                     DJVUDocument *document, int ipage, QObject *parent)
    : DJVUAnnotation(annotation, document, ipage, parent)
{
}

DJVUTextAnnotation::~DJVUTextAnnotation()
{
}

void DJVUTextAnnotation::attach(DJVUDocument *document, DJVUSelection *selection)
{
    if (!selection)
        return;

    QPair<int, QRectF> word = selection->rectAt(0);
    attachAt(document, word.first, word.second.x(), word.second.y());
}

void DJVUTextAnnotation::attachAt(DJVUDocument *document,
                                 unsigned int page, qreal x, qreal y)
{
    attachOnce(document, page);

    // The size of the icon used to render the note on the document
    // is hard-coded in Poppler to 24 1/72 of inch, see AnnotText::draw()
    // of poppler/Annot.cc file. So we use the same size for the bounding box.
    d->m_annotation->setBoundary(QRectF(x, y, 24., 24.));
    d->m_document->addAnnotation(d->m_annotation, d->m_page, true);
    emit attached();
}

DJVUTextAnnotation::IconType DJVUTextAnnotation::icon() const
{
    Poppler::TextAnnotation *anno = static_cast<Poppler::TextAnnotation*>(d->m_annotation);
    return DJVUTextAnnotation::IconType(iconNames.indexOf(anno->textIcon()));
}

void DJVUTextAnnotation::setIcon(DJVUTextAnnotation::IconType value)
{
    if (icon() == value)
        return;

    int index = int(value);
    if (index > iconNames.length() || index < 0) {
        qWarning() << QStringLiteral("Wrong icon for text annotation (%1).").arg(index);
        return;
    }
    
    Poppler::TextAnnotation *anno = static_cast<Poppler::TextAnnotation*>(d->m_annotation);
    anno->setTextIcon(iconNames[value]);
    emit iconChanged();
    
    if (d->m_document != nullptr && d->m_page >= 0)
        d->m_document->onPageModified(d->m_page, d->m_annotation->boundary());
}

DJVUCaretAnnotation::DJVUCaretAnnotation(QObject *parent)
    : DJVUAnnotation(new Poppler::CaretAnnotation(), parent)
{
    Poppler::CaretAnnotation *anno = static_cast<Poppler::CaretAnnotation*>(d->m_annotation);
    anno->setCaretSymbol(Poppler::CaretAnnotation::CaretSymbol::P);
}

DJVUCaretAnnotation::DJVUCaretAnnotation(Poppler::CaretAnnotation *annotation,
                                       DJVUDocument *document, int ipage, QObject *parent)
    : DJVUAnnotation(annotation, document, ipage, parent)
{
}

DJVUCaretAnnotation::~DJVUCaretAnnotation()
{
}

DJVUHighlightAnnotation::DJVUHighlightAnnotation(QObject *parent)
    : DJVUAnnotation(new Poppler::HighlightAnnotation(), parent)
{
    setColor(QColor(255, 255, 0, 255));
}

DJVUHighlightAnnotation::DJVUHighlightAnnotation(Poppler::HighlightAnnotation *annotation,
                                               DJVUDocument *document, int ipage,
                                               QObject *parent)
    : DJVUAnnotation(annotation, document, ipage, parent)
{
}

DJVUHighlightAnnotation::~DJVUHighlightAnnotation()
{
}

DJVUHighlightAnnotation::HighlightType DJVUHighlightAnnotation::style() const
{
    Poppler::HighlightAnnotation *anno = static_cast<Poppler::HighlightAnnotation*>(d->m_annotation);
    return DJVUHighlightAnnotation::HighlightType(anno->highlightType());
}

void DJVUHighlightAnnotation::setStyle(DJVUHighlightAnnotation::HighlightType value)
{
    if (style() == value)
        return;
    
    Poppler::HighlightAnnotation *anno = static_cast<Poppler::HighlightAnnotation*>(d->m_annotation);
    anno->setHighlightType(Poppler::HighlightAnnotation::HighlightType(value));
    emit styleChanged();
    
    if (d->m_document != nullptr && d->m_page >= 0)
        d->m_document->onPageModified(d->m_page, d->m_annotation->boundary());
}

void DJVUHighlightAnnotation::attach(DJVUDocument *document, DJVUSelection *selection)
{
    if (!selection)
        return;

    QPair<int, QRectF> word = selection->rectAt(0);
    attachOnce(document, word.first);

    QRectF bounds = word.second;
    QList<Poppler::HighlightAnnotation::Quad> quads;
    int count = selection->count();

    for (int i = 0; i < count; i++) {
        word = selection->rectAt(i);
        Poppler::HighlightAnnotation::Quad quad;
        if (word.first == d->m_page) {
            quad.capEnd = (i == count - 1);
            quad.capStart = (i == 0);
            quad.feather = 0.;
            quad.points[0] = QPointF(word.second.left(), word.second.top());
            quad.points[1] = QPointF(word.second.right(), word.second.top());
            quad.points[2] = QPointF(word.second.right(), word.second.bottom());
            quad.points[3] = QPointF(word.second.left(), word.second.bottom());
            quads.append(quad);
            bounds |= word.second;
        }
    }

    Poppler::HighlightAnnotation *annotation =
        static_cast<Poppler::HighlightAnnotation*>(d->m_annotation);
    annotation->setHighlightQuads(quads);
    d->m_annotation->setBoundary(bounds);
    d->m_document->addAnnotation(d->m_annotation, d->m_page);
    emit attached();
}
