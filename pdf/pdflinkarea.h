/*
 *
 */

#ifndef LINKLAYER_H
#define LINKLAYER_H

#include <QtQuick/QQuickPaintedItem>

class PDFLinkArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QVariantList links READ links WRITE setLinks NOTIFY linksChanged)
    Q_PROPERTY(QSizeF sourceSize READ sourceSize WRITE setSourceSize NOTIFY sourceSizeChanged)
//     Q_PROPERTY(QColor linkColor READ linkColor WRITE setLinkColor NOTIFY linkColorChanged)

public:
    PDFLinkArea(QQuickItem* parent = 0);
    virtual ~PDFLinkArea();
//     virtual void paint(QPainter* painter);

    QVariantList links() const;
    void setLinks(const QVariantList& newLinks);

    QSizeF sourceSize() const;
    void setSourceSize( const QSizeF& size );

    QColor linkColor() const;
    void setLinkColor( const QColor& color );

Q_SIGNALS:
    void linksChanged();
    void clicked();
    void doubleClicked();
    void linkClicked(QUrl linkTarget);
    void sourceSizeChanged();
    void linkColorChanged();

protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

private:
    class Private;
    Private* d;
};

#endif // LINKLAYER_H
