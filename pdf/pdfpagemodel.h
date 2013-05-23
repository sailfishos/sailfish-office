/*
 *
 */

#ifndef PDFPAGEMODEL_H
#define PDFPAGEMODEL_H

#include <QAbstractListModel>
#include <QImage>

class PDFDocument;
class PDFPageModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(uint pageWidth READ pageWidth WRITE setPageWidth NOTIFY pageWidthChanged)
    Q_PROPERTY(PDFDocument* document READ document WRITE setDocument NOTIFY documentChanged)

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

    PDFDocument* document() const;

public Q_SLOTS:
    void setPageWidth(uint pageWidth);
    void discard( int index );
    void setDocument( PDFDocument* document );

Q_SIGNALS:
    void pageWidthChanged();
    void documentChanged();

private:
    class Private;
    Private * const d;

private Q_SLOTS:
    void documentLoaded();
    void pageFinished(int index, QImage image);
};

#endif // PDFPAGEMODEL_H
