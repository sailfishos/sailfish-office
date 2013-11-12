/*
 *
 */

#ifndef PDFCANVAS_H
#define PDFCANVAS_H

#include <QtQuick/QQuickItem>

class PDFDocument;
class PDFCanvas : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY( PDFDocument* document READ document WRITE setDocument NOTIFY documentChanged )
    Q_PROPERTY( QQuickItem* flickable READ flickable WRITE setFlickable NOTIFY flickableChanged )
    Q_PROPERTY( float spacing READ spacing WRITE setSpacing NOTIFY spacingChanged )

public:
    PDFCanvas(QQuickItem* parent = 0);
    ~PDFCanvas();;

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

    void layout();

Q_SIGNALS:
    void documentChanged();
    void flickableChanged();
    void spacingChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);
    virtual QSGNode* updatePaintNode(QSGNode* node, UpdatePaintNodeData* );

private Q_SLOTS:
    void pageFinished( int id, const QImage& image );
    void documentLoaded();
    void resizeTimeout();
    void pageSizesFinished(const QList< QSizeF >& sizes);

private:
    class Private;
    Private * const d;
};

#endif // PDFCANVAS_H
