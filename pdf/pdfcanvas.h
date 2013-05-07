/*
 *
 */

#ifndef PDFCANVAS_H
#define PDFCANVAS_H

#include <QDeclarativeItem>

class PDFCanvas : public QDeclarativeItem
{
    Q_OBJECT
public:
    PDFCanvas(QDeclarativeItem* parent = 0);
    ~PDFCanvas();

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* );

    Q_INVOKABLE qreal pagePosition( int index ) const;

private Q_SLOTS:
    void pageFinished( int id, const QImage& image );
    void documentLoaded();
    
private:
    class Private;
    Private * const d;
};

#endif // PDFCANVAS_H
