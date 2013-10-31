/*
 *
 */

#ifndef PDFCANVAS_H
#define PDFCANVAS_H

#include <QQuickPaintedItem>

class PDFDocument;
class PDFCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY( PDFDocument* document READ document WRITE setDocument NOTIFY documentChanged )
    Q_PROPERTY( QQuickItem* flickable READ flickable WRITE setFlickable NOTIFY flickableChanged )

public:
    PDFCanvas(QQuickItem* parent = 0);
    ~PDFCanvas();

    virtual void paint(QPainter* painter);

    Q_INVOKABLE qreal pagePosition( int index ) const;

    QQuickItem *flickable() const;
    void setFlickable(QQuickItem *f);

    PDFDocument* document() const;
    void setDocument( PDFDocument* doc );

Q_SIGNALS:
    void documentChanged();
    void flickableChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private Q_SLOTS:
    void pageFinished( int id, const QImage& image );
    void documentLoaded();

private:
    class Private;
    Private * const d;
};

#endif // PDFCANVAS_H
