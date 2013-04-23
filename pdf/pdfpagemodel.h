/*
 *
 */

#ifndef PDFPAGEMODEL_H
#define PDFPAGEMODEL_H

#include <QAbstractListModel>
#include <QImage>

class PDFPageModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(uint pageWidth READ pageWidth WRITE setPageWidth NOTIFY pageWidthChanged)

public:
    enum Roles {
        PageRole = Qt::UserRole,
        WidthRole,
        HeightRole,
    };

    PDFPageModel(QObject* parent = 0);
    virtual ~PDFPageModel();

    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent) const;

    uint pageWidth() const;

public Q_SLOTS:
    void setPageWidth(uint pageWidth);
    void discard( int index );

Q_SIGNALS:
    void pageWidthChanged();

private:
    class Private;
    Private * const d;

private Q_SLOTS:
    void documentLoaded();
    void pageFinished(int index, QImage image);
};

#endif // PDFPAGEMODEL_H
