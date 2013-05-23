/*
 *
 */

#ifndef PDFCANVAS_H
#define PDFCANVAS_H

#include <QDeclarativeItem>

class PDFDocument;
class PDFCanvas : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY( PDFDocument* document READ document WRITE setDocument NOTIFY documentChanged )

public:
    PDFCanvas(QDeclarativeItem* parent = 0);
    ~PDFCanvas();

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* );

    Q_INVOKABLE qreal pagePosition( int index ) const;

    PDFDocument* document() const;
    void setDocument( PDFDocument* doc );

Q_SIGNALS:
    void documentChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private Q_SLOTS:
    void pageFinished( int id, const QImage& image );
    void documentLoaded();
    void setRenderThreadWidth();
    
private:
    class Private;
    Private * const d;
};

#endif // PDFCANVAS_H
