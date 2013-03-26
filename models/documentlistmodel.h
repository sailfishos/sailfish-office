/*
 *
 */

#ifndef DOCUMENTLISTMODEL_H
#define DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <qdatetime.h>

class DocumentListModelPrivate;

class DocumentListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        FileTypeRole,
        FileSizeRole,
        FileReadRole,
        FileMimeTypeRole,
        FileDocumentClass
    };

    DocumentListModel( QObject* parent = 0 );
    ~DocumentListModel();

    DocumentListModel( const DocumentListModel& ) = delete;
    DocumentListModel& operator=( const DocumentListModel& ) = delete;

    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent) const;

    void addItem(QString name, QString path, QString type, int size, QDateTime lastRead, QString mimeType);
    void removeAt(int index);
    void clear();

private:
    class Private;
    const QScopedPointer< Private > d;
};

#endif // DOCUMENTLISTMODEL_H
