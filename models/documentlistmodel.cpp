/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Contact: Robin Burchell <robin.burchell@jolla.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "documentlistmodel.h"

#include <QDir>
#include <QSet>

struct DocumentListModelEntry
{
    QString fileName;
    QString filePath;
    QString fileType;
    int fileSize;
    QDateTime fileRead;
    QString mimeType;
    QSet<QString> tags;
    TagsThreadJob *job;
    DocumentListModel::DocumentClass documentClass;
    bool dirty; // When true, should be removed from list.
};

class DocumentListModel::Private
{
public:
    Private()
    {
        roles.insert(FileNameRole, "fileName");
        roles.insert(FilePathRole, "filePath");
        roles.insert(FileTypeRole, "fileType");
        roles.insert(FileSizeRole, "fileSize");
        roles.insert(FileReadRole, "fileRead");
        roles.insert(FileMimeTypeRole, "fileMimeType");
        roles.insert(FileDocumentClass, "fileDocumentClass");
    }
    QList<DocumentListModelEntry> entries;
    QHash< int, QByteArray > roles;
    TagsThread *tagsThread;
    TagListModel tagsModel;
};

DocumentListModel::DocumentListModel(QObject *parent)
    : QAbstractListModel(parent), d(new Private)
{
    d->tagsThread = new TagsThread( this );
    connect( d->tagsThread, &TagsThread::jobFinished, this, &DocumentListModel::jobFinished );
}

DocumentListModel::~DocumentListModel()
{
    delete d->tagsThread;
}

QVariant DocumentListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= d->entries.count())
        return QVariant();
    
    switch (role) {
    case FileNameRole:
        return d->entries.at(index.row()).fileName;
    case FilePathRole:
        return d->entries.at(index.row()).filePath;
    case FileTypeRole:
        return QString().append(d->entries.at(index.row()).fileType).append(d->entries.at(index.row()).fileRead.toString(Qt::ISODate));
    case FileSizeRole:
        return d->entries.at(index.row()).fileSize;
    case FileReadRole:
        return d->entries.at(index.row()).fileRead;
    case FileMimeTypeRole:
        return d->entries.at(index.row()).mimeType;
    case FileDocumentClass:
        return d->entries.at(index.row()).documentClass;
    default:
        break;
    }

    return QVariant();
}
bool DocumentListModel::hasTagAt(int row, const QString &tag) const
{
    if (row < 0 && row >= d->entries.count())
      return false;

    return d->entries.at(row).tags.contains(tag);
}
bool DocumentListModel::hasTag(const QString &path, const QString &tag) const
{
    for (QList<DocumentListModelEntry>::iterator entry = d->entries.begin();
         entry != d->entries.end(); entry++) {
        if ( path == entry->filePath )
            return entry->tags.contains(tag);
    }

    return false;
}
void DocumentListModel::addTag(const QString &path, const QString &tag)
{
    int row = 0;
    for (QList<DocumentListModelEntry>::iterator entry = d->entries.begin();
         entry != d->entries.end(); entry++) {
        if ( path == entry->filePath ) {
            if ( !entry->tags.contains(tag) ) {
                entry->tags.insert(tag);
                dataChanged(index(row), index(row));
                d->tagsModel.addItem(tag);
            }
            return;
        }
        row += 1;
    }
}
void DocumentListModel::removeTag(const QString &path, const QString &tag)
{
    int row = 0;
    for (QList<DocumentListModelEntry>::iterator entry = d->entries.begin();
         entry != d->entries.end(); entry++) {
        if ( path == entry->filePath ) {
            if ( entry->tags.contains(tag) ) {
                entry->tags.remove(tag);
                dataChanged(index(row), index(row));
                d->tagsModel.removeItem(tag);
            }
            return;
        }
        row += 1;
    }
}
TagListModel* DocumentListModel::tags() const
{
    return &d->tagsModel;
}

int DocumentListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return d->entries.count();
}

QHash<int, QByteArray> DocumentListModel::roleNames() const
{
    return d->roles;
}

void DocumentListModel::setAllItemsDirty(bool status)
{
    for (QList<DocumentListModelEntry>::iterator entry = d->entries.begin(); entry != d->entries.end(); entry++) {
        entry->dirty = status;
    }
}

void DocumentListModel::addItem(QString name, QString path, QString type, int size, QDateTime lastRead, QString mimeType)
{
    // We sometimes get duplicate entries... and that's kind of silly.
    for (QList<DocumentListModelEntry>::iterator entry = d->entries.begin();
        entry != d->entries.end(); entry++) {
        if (entry->filePath == path) {
            entry->dirty = false;
            entry->fileType = type;
            entry->fileSize = size;
            entry->fileRead = lastRead;
            entry->mimeType = mimeType;
            entry->documentClass = static_cast<DocumentClass>(mimeTypeToDocumentClass(mimeType));
            return;
        }
    }

    DocumentListModelEntry entry;
    entry.dirty    = false;
    entry.fileName = name;
    entry.filePath = path;
    entry.fileType = type;
    entry.fileSize = size;
    entry.fileRead = lastRead;
    entry.mimeType = mimeType;
    entry.documentClass = static_cast<DocumentClass>(mimeTypeToDocumentClass(mimeType));
    entry.job = new TagsThreadJob(path);
    //entry.job.setTarget(entry.tags);
    d->tagsThread->queueJob(entry.job);

    int index = 0;
    for (; index < d->entries.count(); ++index) {
        if (d->entries.at(index).fileRead < entry.fileRead)
            break;
    }

    beginInsertRows(QModelIndex(), index, index);
    d->entries.insert(index, entry);
    endInsertRows();
}

void DocumentListModel::removeItemsDirty()
{
    for (int index=0; index < d->entries.count(); index++) {
        if (d->entries.at(index).dirty) {
            removeAt(index);
        }
    }
}


void DocumentListModel::removeAt(int index)
{
    if (index > -1 && index < d->entries.count()) {
        d->tagsThread->cancelJob(d->entries.at(index).job);
        beginRemoveRows(QModelIndex(), index, index);
        d->entries.removeAt(index);
        endRemoveRows();
    }
}

void DocumentListModel::clear()
{
    d->tagsThread->cancelJob(0);
    beginResetModel();
    d->entries.clear();
    endResetModel();
}

void DocumentListModel::jobFinished(TagsThreadJob *job)
{
    int row = 0;
    for(QList<DocumentListModelEntry>::iterator entry = d->entries.begin();
        entry != d->entries.end(); entry++) {
        if (entry->filePath == job->path) {
            entry->job  = 0;
            entry->tags.clear();
            for (int i=0; i < job->tags.count(); i++) {
                entry->tags.insert(job->tags.at(i));
                d->tagsModel.addItem(job->tags.at(i));
            }
            dataChanged(index(row), index(row));
            break;
        }
        row += 1;
    }
    job->deleteLater();
}

int DocumentListModel::mimeTypeToDocumentClass(QString mimeType) const
{
    DocumentClass documentClass = UnknownDocument;
    if (mimeType == QLatin1String("application/vnd.oasis.opendocument.text")
            || mimeType == QLatin1String("application/msword")
            || mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.wordprocessingml.document")
            || mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.wordprocessingml.template")
            || mimeType == QLatin1String("application/vnd.ms-word.document.macroEnabled.12")
            || mimeType == QLatin1String("application/vnd.ms-word.template.macroEnabled.12")) {
        documentClass = TextDocument;
    } else if (mimeType == QLatin1String("application/vnd.oasis.opendocument.presentation")
               || mimeType == QLatin1String("application/vnd.ms-powerpoint")
               || mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.presentationml.presentation")
               || mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.presentationml.template")
               || mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.presentationml.slideshow")
               || mimeType == QLatin1String("application/vnd.ms-powerpoint.presentation.macroEnabled.12")
               || mimeType == QLatin1String("application/vnd.ms-powerpoint.template.macroEnabled.12")
               || mimeType == QLatin1String("application/vnd.ms-powerpoint.slideshow.macroEnabled.12") ) {
        documentClass = PresentationDocument;
    } else if (mimeType == QLatin1String("application/vnd.oasis.opendocument.spreadsheet")
               || mimeType == QLatin1String("application/vnd.ms-excel")
               || mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet")
               || mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.spreadsheetml.template")
               || mimeType == QLatin1String("application/vnd.ms-excel.sheet.macroEnabled")
               || mimeType == QLatin1String("application/vnd.ms-excel.sheet.macroEnabled.12")
               || mimeType == QLatin1String("application/vnd.ms-excel.template.macroEnabled.12") ) {
        documentClass = SpreadSheetDocument;
    } else if (mimeType == QLatin1String("application/pdf")) {
        documentClass = PDFDocument;
    }

    return documentClass;
}
