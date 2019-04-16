/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Contact: Robin Burchell <robin.burchell@jolla.com>
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

#ifndef PDFCANVAS_H
#define PDFCANVAS_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGTexture>

#include <poppler-qt5.h>

class PDFDocument;

class PDFCanvas : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(PageRotation)
    Q_PROPERTY(PDFDocument* document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(QQuickItem* flickable READ flickable WRITE setFlickable NOTIFY flickableChanged)
    Q_PROPERTY(float spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(QColor linkColor READ linkColor WRITE setLinkColor NOTIFY linkColorChanged)
    Q_PROPERTY(QColor pagePlaceholderColor READ pagePlaceholderColor WRITE setPagePlaceholderColor NOTIFY pagePlaceholderColorChanged)
    Q_PROPERTY(int currentPage READ currentPage NOTIFY currentPageChanged)
    Q_PROPERTY(float linkWiggle READ linkWiggle WRITE setLinkWiggle NOTIFY linkWiggleChanged)
    Q_PROPERTY(PageRotation pageRotation READ pageRotation WRITE setPageRotation NOTIFY pageRotationChanged)

public:
    enum PageRotation {
        NoRotation,
        Clockwise,
        CounterClockwise,
        UpSideDown
    };

    PDFCanvas(QQuickItem *parent = 0);
    ~PDFCanvas();

    typedef QPair<int, QRectF> ReducedBox;

    Q_INVOKABLE QRectF pageRectangle(int index) const;

    QQuickItem *flickable() const;
    void setFlickable(QQuickItem *f);

    PDFDocument* document() const;
    void setDocument(PDFDocument *doc);

    /**
     * Getter for property #spacing.
     */
    float spacing() const;
    /**
     * Setter for property #spacing.
     */
    void setSpacing(float newValue);

    float linkWiggle() const;
    void setLinkWiggle(float newValue);

    /**
     * Getter for property #linkColor.
     */
    QColor linkColor() const;
    /**
     * Setter for property #linkColor.
     */
    void setLinkColor(const QColor &color);
    /**
     * Getter for property #pagePlaceholderColor.
     */
    QColor pagePlaceholderColor() const;
    /**
     * Setter for property #pagePlaceholderColor.
     */
    void setPagePlaceholderColor(const QColor &color);

    /**
     * Getter for property #currentPage.
     */
    int currentPage() const;

    PageRotation pageRotation() const;
    void setPageRotation(PageRotation pageRotation);
    Q_INVOKABLE void rotate(bool clockwise);

    void layout();

    /**
     * \return The url of the link at point or an empty url if there is no link at point.
     */
    QPair<QUrl, ReducedBox> urlAtPoint(const QPointF &point) const;
    QPair<int, QRectF> pageAtPoint(const QPointF &point) const;
    QPair<Poppler::Annotation*, ReducedBox> annotationAtPoint(const QPointF &point) const;
    /**
     * \return A rectangle in the canvas coordinates from a rectangle
     * in page coordinates. Index is the index of the page.
     */
    Q_INVOKABLE QRectF fromPageToItem(int index, const QRectF &rect) const;
    Q_INVOKABLE QPointF fromPageToItem(int index, const QPointF &point) const;
    QSizeF fromPageToItem(int index, const QSizeF &size) const;
    QPointF fromItemToPage(const QRectF &page, const QPointF &point) const;
    /**
     * Provide a distance measure from @point to a rectangle given by @reducedCoordRect.
     * @point is given in PDFCanvas coordinates, while @reducedCoordRect is in
     * reduced coordinates and will be converted to PDFCanvas coordinates thanks
     * to @pageRect. @pageRect can be obtained by calling pageAtPoint().
     */
    qreal squaredDistanceFromRect(const QRectF &pageRect,
                                  const QRectF &reducedCoordRect,
                                  const QPointF &point) const;

Q_SIGNALS:
    void documentChanged();
    void flickableChanged();
    void spacingChanged();
    void linkColorChanged();
    void pagePlaceholderColorChanged();
    void currentPageChanged();
    void pageLayoutChanged();
    void linkWiggleChanged();
    void pageRotationChanged();

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual QSGNode* updatePaintNode(QSGNode *node, UpdatePaintNodeData*);

private Q_SLOTS:
    void linksFinished(int id, const QList<QPair<QRectF, QUrl> > &links);
    void pageModified(int id, const QRectF &subpart);
    void pageFinished(int id, int pageRenderWidth,
                      QRect subpart, QSGTexture *texture, int extraData);
    void documentLoaded();
    void resizeTimeout();
    void pageSizesFinished(const QList<QSizeF> &sizes);
    void sceneGraphInvalidated();

private:
    class Private;
    Private * const d;
};

Q_DECLARE_METATYPE(PDFCanvas*)

#endif // PDFCANVAS_H
