/*
 *
 */

#ifndef DOCUMENTLISTMODEL_H
#define DOCUMENTLISTMODEL_H

#include <QAbstractListModel>

class DocumentListModelPrivate;

class DocumentListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
public:
    enum Roles
    {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        FileTypeRole,
        FileSizeRole
    };
    
    DocumentListModel( QObject* parent = 0 );
    ~DocumentListModel();
    
    DocumentListModel( const DocumentListModel& ) = delete;
    DocumentListModel& operator=( const DocumentListModel& ) = delete;
    
    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent) const;
    
    QString path() const;
    void setPath(const QString& newPath);

Q_SIGNALS:
    void pathChanged();

private:
    class Private;
    const QScopedPointer< Private > d;
};

#endif // DOCUMENTLISTMODEL_H
