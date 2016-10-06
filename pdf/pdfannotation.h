/*
 * Copyright (C) 2016 Caliste Damien.
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

#ifndef PDFANNOTATION_H
#define PDFANNOTATION_H

#include <poppler-qt5.h>

#include "pdfdocument.h"
#include "pdfselection.h"

class PDFTextAnnotation;
class PDFHighlightAnnotation;

class PDFAnnotation : public QObject
{
    Q_OBJECT

    Q_ENUMS(SubType)

    Q_PROPERTY(PDFDocument* document READ document NOTIFY attached)
    Q_PROPERTY(int page READ page NOTIFY attached)
    Q_PROPERTY(QRectF boundary READ boundary NOTIFY attached)

    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    Q_PROPERTY(QString contents READ contents WRITE setContents NOTIFY contentsChanged)
    Q_PROPERTY(QDateTime creationDate READ creationDate CONSTANT)
    Q_PROPERTY(QDateTime modificationDate READ modificationDate NOTIFY modificationDateChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(SubType type READ type CONSTANT)

public:
    enum SubType {
        Base,
        Text,
        Line,
        GeometricFigure,
        Highlight,
        Stamp,
        InkPath,
        Link,
        Caret,
        FileAttachment,
        Sound,
        Movie,
        Screen,
        Widget
    };

    PDFAnnotation(QObject *parent = 0);
    PDFAnnotation(Poppler::Annotation *annotation, QObject *parent = 0);
    PDFAnnotation(Poppler::Annotation *annotation,
                  PDFDocument *document, int ipage, QObject *parent = 0);
    ~PDFAnnotation();

    SubType type() const;

    PDFDocument* document() const;
    int page() const;
    QRectF boundary() const;
    Q_INVOKABLE void attach(PDFDocument *document, PDFSelection *selection);
    Q_INVOKABLE void remove();

    QString author() const;
    void setAuthor(const QString &value);

    QString contents() const;
    void setContents(const QString &value);

    QDateTime creationDate() const;

    QDateTime modificationDate() const;

    QColor color() const;
    void setColor(const QColor &value);

Q_SIGNALS:
    void attached();
    void authorChanged();
    void contentsChanged();
    void creationDateChanged();
    void modificationDateChanged();
    void colorChanged();

protected:
    class Private;
    Private *d;

    void attachOnce(PDFDocument *document, int page);
};

class PDFTextAnnotation : public PDFAnnotation
{
    Q_OBJECT

    Q_ENUMS(IconType)

    Q_PROPERTY(IconType icon READ icon WRITE setIcon NOTIFY iconChanged)

public:
    enum IconType {
        Note,
        Comment,
        Key,
        Help,
        NewParagraph,
        Paragraph,
        Insert,
        Cross,
        Circle
    };

    PDFTextAnnotation(QObject *parent = 0);
    PDFTextAnnotation(Poppler::TextAnnotation *annotation,
                      PDFDocument *document, int ipage, QObject *parent = 0);
    ~PDFTextAnnotation();

    Q_INVOKABLE void attach(PDFDocument *document, PDFSelection *selection);
    Q_INVOKABLE void attachAt(PDFDocument *document,
                              unsigned int page, qreal x, qreal y);

    IconType icon() const;
    void setIcon(IconType value);

Q_SIGNALS:
    void iconChanged();
};

class PDFCaretAnnotation : public PDFAnnotation
{
    Q_OBJECT

public:
    PDFCaretAnnotation(QObject *parent = 0);
    PDFCaretAnnotation(Poppler::CaretAnnotation *annotation,
                       PDFDocument *document, int ipage, QObject *parent = 0);
    ~PDFCaretAnnotation();
};

class PDFHighlightAnnotation : public PDFAnnotation
{
    Q_OBJECT

    Q_ENUMS(HighlightType)

    Q_PROPERTY(HighlightType style READ style WRITE setStyle NOTIFY styleChanged)

public:
    enum HighlightType {
        Highlight,
        Squiggly,
        Underline,
        StrikeOut
    };

    PDFHighlightAnnotation(QObject *parent = 0);
    PDFHighlightAnnotation(Poppler::HighlightAnnotation *annotation,
                           PDFDocument *document, int ipage, QObject *parent = 0);
    ~PDFHighlightAnnotation();

    Q_INVOKABLE void attach(PDFDocument *document, PDFSelection *selection);

    HighlightType style() const;
    void setStyle(HighlightType value);

Q_SIGNALS:
    void styleChanged();
};

#endif // PDFANNOTATION_H
