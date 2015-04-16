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

class PDFDocument;
class PDFCanvas : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY( PDFDocument* document READ document WRITE setDocument NOTIFY documentChanged )
    Q_PROPERTY( QQuickItem* flickable READ flickable WRITE setFlickable NOTIFY flickableChanged )
    Q_PROPERTY( float spacing READ spacing WRITE setSpacing NOTIFY spacingChanged )
    Q_PROPERTY( QColor linkColor READ linkColor WRITE setLinkColor NOTIFY linkColorChanged )
    Q_PROPERTY( int currentPage READ currentPage NOTIFY currentPageChanged )

public:
    PDFCanvas(QQuickItem* parent = 0);
    ~PDFCanvas();

    Q_INVOKABLE qreal pagePosition( int index ) const;

    QQuickItem *flickable() const;
    void setFlickable(QQuickItem *f);

    PDFDocument* document() const;
    void setDocument( PDFDocument* doc );

    /**
     * Getter for property #spacing.
     */
    float spacing() const;
    /**
     * Setter for property #spacing.
     */
    void setSpacing(float newValue);

    /**
     * Getter for property #linkColor.
     */
    QColor linkColor() const;
    /**
     * Setter for property #linkColor.
     */
    void setLinkColor( const QColor& color );

    /**
     * Getter for property #currentPage.
     */
    int currentPage() const;

    void layout();

    /**
     * \return The url of the link at point or an empty url if there is no link at point.
     */
    QUrl urlAtPoint( const QPoint& point );

Q_SIGNALS:
    void documentChanged();
    void flickableChanged();
    void spacingChanged();
    void linkColorChanged();
    void currentPageChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);
    virtual QSGNode* updatePaintNode(QSGNode* node, UpdatePaintNodeData* );

private Q_SLOTS:
    void pageFinished( int id, QSGTexture *texture );
    void documentLoaded();
    void resizeTimeout();
    void pageSizesFinished(const QList< QSizeF >& sizes);
    void sceneGraphInvalidated();

private:
    class Private;
    Private * const d;
};

Q_DECLARE_METATYPE( PDFCanvas* )

#endif // PDFCANVAS_H
