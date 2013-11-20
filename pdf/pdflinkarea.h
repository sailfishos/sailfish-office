/*
 *
 */

#ifndef LINKLAYER_H
#define LINKLAYER_H

#include <QtQuick/QQuickPaintedItem>

class PDFCanvas;
class PDFLinkArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY( PDFCanvas* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged )

public:
    PDFLinkArea(QQuickItem* parent = 0);
    virtual ~PDFLinkArea();

    PDFCanvas* canvas() const;
    void setCanvas( PDFCanvas* newCanvas );

Q_SIGNALS:
    void clicked();
    void doubleClicked();
    void linkClicked(QUrl linkTarget);

    void canvasChanged();

protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

private:
    class Private;
    Private* d;
};

#endif // LINKLAYER_H
