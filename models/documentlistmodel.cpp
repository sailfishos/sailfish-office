/*
 *
 */

#include "documentlistmodel.h"

#include <QDir>

class DocumentListModel::Private
{
public:
    QDir directory;
    QFileInfoList entries;
};

DocumentListModel::DocumentListModel(QObject* parent)
    : QAbstractListModel(parent), d(new Private)
{
    QHash< int, QByteArray > roles;
    roles.insert( FileNameRole, "fileName" );
    roles.insert( FilePathRole, "filePath" );
    roles.insert( FileTypeRole, "fileType" );
    roles.insert( FileSizeRole, "fileSize" );
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
            return d->entries.at( index.row() ).fileName().split( '.' ).at( 0 );
        case FilePathRole:
            return d->entries.at( index.row() ).filePath();
        case FileTypeRole:
            return d->entries.at( index.row() ).fileName().split( '.' ).at( 1 );
        case FileSizeRole:
            return QString( "%1 KiB" ).arg( d->entries.at( index.row() ).size() / 1024 );
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

QString DocumentListModel::path() const
{
    return d->directory.path();
}

void DocumentListModel::setPath(const QString& newPath)
{
    if( newPath != d->directory.path() )
    {
        d->directory.setPath( newPath );
        beginRemoveRows( QModelIndex(), 0, d->entries.count() );
        d->entries.clear();
        endRemoveRows();
        d->entries = d->directory.entryInfoList( QStringList() << "*.odt" << "*.ods" << "*.odp", QDir::Files, QDir::Name );
        beginInsertRows( QModelIndex(), 0, d->entries.count() );
        endInsertRows();
        emit pathChanged();
    }
}

#include "documentlistmodel.moc"
