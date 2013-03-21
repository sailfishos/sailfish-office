/*
 *
 */

#include "documentlistmodel.h"

#include <QDir>

struct DocumentListModelEntry
{
    QString fileName;
    QString filePath;
    QString fileType;
    int fileSize;
    QDateTime fileRead;
};

class DocumentListModel::Private
{
public:
    QList<DocumentListModelEntry> entries;
};

DocumentListModel::DocumentListModel(QObject* parent)
    : QAbstractListModel(parent), d(new Private)
{
    QHash< int, QByteArray > roles;
    roles.insert( FileNameRole, "fileName" );
    roles.insert( FilePathRole, "filePath" );
    roles.insert( FileTypeRole, "fileType" );
    roles.insert( FileSizeRole, "fileSize" );
    roles.insert( FileReadRole, "fileRead" );
    setRoleNames( roles );
}

DocumentListModel::~DocumentListModel()
{
}

QVariant DocumentListModel::data(const QModelIndex& index, int role) const
{
    if( !index.isValid() || index.row() < 0 || index.row() >= d->entries.count() )
        return QVariant();
    
    switch( role )
    {
        case FileNameRole:
            return d->entries.at( index.row() ).fileName;
        case FilePathRole:
            return d->entries.at( index.row() ).filePath;
        case FileTypeRole:
            return d->entries.at( index.row() ).fileType;
        case FileSizeRole:
            return QString( "%1 KiB" ).arg( d->entries.at( index.row() ).fileSize / 1024 );
        case FileReadRole:
            return d->entries.at( index.row() ).fileRead;
        default:
            break;
    }
    
    return QVariant();
}

int DocumentListModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->entries.count();
}

void DocumentListModel::addItem(QString name, QString path, QString type, int size, QDateTime lastRead)
{
    DocumentListModelEntry entry;
    entry.fileName = name;
    entry.filePath = path;
    entry.fileType = type;
    entry.fileSize = size;
    entry.fileRead = lastRead;
    beginInsertRows(QModelIndex(), d->entries.count(), d->entries.count());
    d->entries.append(entry);
    endInsertRows();
}

void DocumentListModel::removeAt(int index)
{
    if(index > -1 && index < d->entries.count())
    {
        beginRemoveRows(QModelIndex(), index, index);
        d->entries.removeAt(index);
        endRemoveRows();
    }
}

void DocumentListModel::clear()
{
    beginResetModel();
    d->entries.clear();
    endResetModel();
}

#include "documentlistmodel.moc"
