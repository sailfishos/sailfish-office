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
    QHash<int, QByteArray> roles;
    TagsThread *tagsThread; // To delegate tag storage with SQL backend.
    TrackerTagProvider trackerTag;
    TagListModel tagsModel; // A QML list of all tags.
    QHash<QString, QSet<QString>> tags; // The association tag <-> [set of filenames]
};

DocumentListModel::DocumentListModel(QObject *parent)
    : QAbstractListModel(parent), d(new Private)
{
    // d->tagsThread = new TagsThread(this);
    // connect(d->tagsThread, &TagsThread::jobFinished,
    //         this, &DocumentListModel::jobFinished);
    connect(&d->trackerTag, &TrackerTagProvider::tagLoaded,
            this, &DocumentListModel::tagLoaded);
}

DocumentListModel::~DocumentListModel()
{
    // delete d->tagsThread;
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
void DocumentListModel::notifyForPath(const QString &path)
{
    int row = 0;
    for (QList<DocumentListModelEntry>::iterator entry = d->entries.begin();
         entry != d->entries.end(); entry++) {
        if (path == entry->filePath) {
            dataChanged(index(row), index(row));
            return;
        }
        row += 1;
    }
}
bool DocumentListModel::hasTag(int row, const QString &tag) const
{
    if (row < 0 && row >= d->entries.count())
      return false;

    return hasTag(d->entries.at(row).filePath, tag);
}
bool DocumentListModel::hasTag(const QString &path, const QString &tag) const
{
    return d->tags.value(tag).contains(path);
}
void DocumentListModel::addTag(const QString &path, const QString &tag)
{
    QSet<QString> &files = d->tags[tag];
    if (files.contains(path))
        return; // This path has already this tag.

    files.insert(path);
    // TagsThreadJob *job = new TagsThreadJob(path, TagsThreadJob::TaskAddTags);
    // job->tags.append(tag);
    // d->tagsThread->queueJob(job);
    d->trackerTag.addTag(path, tag);
    d->tagsModel.addItem(tag);
    notifyForPath(path);
}
void DocumentListModel::removeTag(const QString &path, const QString &tag)
{
    QSet<QString> &files = d->tags[tag];
    if (!files.contains(path))
        return; // This path has not this tag.

    files.remove(path);
    if (files.empty())
        d->tags.remove(tag);
    // TagsThreadJob *job = new TagsThreadJob(path, TagsThreadJob::TaskRemoveTags);
    // job->tags.append(tag);
    // d->tagsThread->queueJob(job);
    d->trackerTag.removeTag(path, tag);
    d->tagsModel.removeItem(tag);
    notifyForPath(path);
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
    d->trackerTag.loadTags(path);
    //d->tagsThread->queueJob(new TagsThreadJob(path, TagsThreadJob::TaskLoadTags));

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
        // d->tagsThread->cancelJobsForPath(d->entries.at(index).filePath);
        beginRemoveRows(QModelIndex(), index, index);
        d->entries.removeAt(index);
        endRemoveRows();
    }
}

void DocumentListModel::clear()
{
    // d->tagsThread->cancelAllJobs();
    beginResetModel();
    d->entries.clear();
    endResetModel();
}

void DocumentListModel::jobFinished(TagsThreadJob *job)
{
    if (job->task == TagsThreadJob::TaskLoadTags) {
        for (QList<QString>::const_iterator tag = job->tags.begin();
             tag != job->tags.end(); tag++) {
            QSet<QString> &files = d->tags[*tag];
            files.insert(job->path);
            d->tagsModel.addItem(*tag);
        }
        notifyForPath(job->path);
    }
    job->deleteLater();
}

void DocumentListModel::tagLoaded(const QString &path, const QList<QString> &tags)
{
    for (QList<QString>::const_iterator tag = tags.begin();
         tag != tags.end(); tag++) {
        QSet<QString> &files = d->tags[*tag];
        files.insert(path);
        d->tagsModel.addItem(*tag);
    }
    notifyForPath(path);
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
