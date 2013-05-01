/*
 *
 */

#ifndef PDFTOCMODEL_H
#define PDFTOCMODEL_H

#include <QtCore/QAbstractListModel>

namespace Poppler { class Document; }
class PDFTocModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum PDFTocModelRoles {
        Title = Qt::UserRole + 1,
        Level,
        PageNumber
    };
    explicit PDFTocModel(Poppler::Document* document, QObject* parent = 0);
    virtual ~PDFTocModel();

    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent) const;

private:
    class Private;
    Private * const d;
};

#endif // PDFTOCMODEL_H
