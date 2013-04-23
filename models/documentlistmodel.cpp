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
    QString mimeType;
    DocumentListModel::DocumentClass documentClass;
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
    roles.insert( FileMimeTypeRole, "fileMimeType" );
    roles.insert( FileDocumentClass, "fileDocumentClass" );
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
            return QString().append(d->entries.at( index.row() ).fileType).append(d->entries.at( index.row() ).fileRead.toString(Qt::ISODate));
        case FileSizeRole:
            return QString( "%1 KiB" ).arg( d->entries.at( index.row() ).fileSize / 1024 );
        case FileReadRole:
            return d->entries.at( index.row() ).fileRead;
        case FileMimeTypeRole:
            return d->entries.at( index.row() ).mimeType;
        case FileDocumentClass:
            return d->entries.at( index.row() ).documentClass;
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

void DocumentListModel::addItem(QString name, QString path, QString type, int size, QDateTime lastRead, QString mimeType)
{
    // We sometimes get duplicate entries... and that's kind of silly.
    foreach(const DocumentListModelEntry& entry, d->entries) {
        if(entry.filePath == path)
            return;
    }

    DocumentListModelEntry entry;
    entry.fileName = name;
    entry.filePath = path;
    entry.fileType = type;
    entry.fileSize = size;
    entry.fileRead = lastRead;
    entry.mimeType = mimeType;

    if(entry.mimeType == QLatin1String("application/vnd.oasis.opendocument.text") ||
       entry.mimeType == QLatin1String("application/msword") ||
       entry.mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.wordprocessingml.document") ||
       entry.mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.wordprocessingml.template") ||
       entry.mimeType == QLatin1String("application/vnd.ms-word.document.macroEnabled.12") ||
       entry.mimeType == QLatin1String("application/vnd.ms-word.template.macroEnabled.12"))
    {
        entry.documentClass = TextDocument;
    }
    else
    if(entry.mimeType == QLatin1String("application/vnd.oasis.opendocument.presentation") ||
       entry.mimeType == QLatin1String("application/vnd.ms-powerpoint") ||
       entry.mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.presentationml.presentation") ||
       entry.mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.presentationml.template") ||
       entry.mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.presentationml.slideshow") ||
       entry.mimeType == QLatin1String("application/vnd.ms-powerpoint.presentation.macroEnabled.12") ||
       entry.mimeType == QLatin1String("application/vnd.ms-powerpoint.template.macroEnabled.12") ||
       entry.mimeType == QLatin1String("application/vnd.ms-powerpoint.slideshow.macroEnabled.12") )
    {
        entry.documentClass = PresentationDocument;
    }
    else
    if(entry.mimeType == QLatin1String("application/vnd.oasis.opendocument.spreadsheet") ||
       entry.mimeType == QLatin1String("application/vnd.ms-excel") ||
       entry.mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet") ||
       entry.mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.spreadsheetml.template") ||
       entry.mimeType == QLatin1String("application/vnd.ms-excel.sheet.macroEnabled") ||
       entry.mimeType == QLatin1String("application/vnd.ms-excel.sheet.macroEnabled.12") ||
       entry.mimeType == QLatin1String("application/vnd.ms-excel.template.macroEnabled.12") )
    {
        entry.documentClass = SpreadSheetDocument;
    }
    else
    if(entry.mimeType == QLatin1String("application/pdf"))
    {
        entry.documentClass = PDFDocument;
    }
    else {
        entry.documentClass = UnknownDocument;
    }

    int index = 0;
    for(; index < d->entries.count(); ++index) {
        if(d->entries.at(index).fileRead < entry.fileRead)
            break;
    }

    beginInsertRows(QModelIndex(), index, index);
    d->entries.insert(index, entry);
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
