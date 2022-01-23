/*
 * Copyright (C) 2019 Open Mobile Platform LLC
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

struct DocumentListModelEntry
{
    QString fileName;
    QString filePath;
    QString fileType;
    int fileSize;
    QDateTime fileDate;
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
        roles.insert(FileDateRole, "fileDate");
        roles.insert(FileMimeTypeRole, "fileMimeType");
        roles.insert(FileDocumentClass, "fileDocumentClass");
        roles.insert(FileTypeAndNameRole, "fileTypeAndNameRole");
    }
    QList<DocumentListModelEntry> entries;
    QHash<int, QByteArray> roles;
};

DocumentListModel::DocumentListModel(QObject *parent)
    : QAbstractListModel(parent), d(new Private)
{
}

DocumentListModel::~DocumentListModel()
{
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
        return d->entries.at(index.row()).fileType;
    case FileSizeRole:
        return d->entries.at(index.row()).fileSize;
    case FileDateRole:
        return d->entries.at(index.row()).fileDate;
    case FileMimeTypeRole:
        return d->entries.at(index.row()).mimeType;
    case FileDocumentClass:
        return d->entries.at(index.row()).documentClass;
    case FileTypeAndNameRole:
        return QString().append(d->entries.at(index.row()).fileType).append(d->entries.at(index.row()).fileName);
    default:
        break;
    }

    return QVariant();
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

void DocumentListModel::addItem(const QString &name, const QString &path, const QString &type, int size, QDateTime date,
                                const QString &mimeType)
{
    // We sometimes get duplicate entries... and that's kind of silly.
    for (QList<DocumentListModelEntry>::iterator entry = d->entries.begin();
        entry != d->entries.end(); entry++) {
        if (entry->filePath == path) {
            entry->dirty = false;
            entry->fileType = type;
            entry->fileSize = size;
            entry->fileDate = date;
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
    entry.fileDate = date;
    entry.mimeType = mimeType;
    entry.documentClass = static_cast<DocumentClass>(mimeTypeToDocumentClass(mimeType));

    int index = 0;
    for (; index < d->entries.count(); ++index) {
        if (d->entries.at(index).fileDate < entry.fileDate)
            break;
    }

    beginInsertRows(QModelIndex(), index, index);
    d->entries.insert(index, entry);
    endInsertRows();
}

void DocumentListModel::removeItemsDirty()
{
    for (int index=0; index < d->entries.count();) {
        if (d->entries.at(index).dirty) {
            beginRemoveRows(QModelIndex(), index, index);
            d->entries.removeAt(index);
            endRemoveRows();
        } else {
            ++index;
        }
    }
}


void DocumentListModel::removeAt(int index)
{
    if (index > -1 && index < d->entries.count()) {
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

int DocumentListModel::mimeTypeToDocumentClass(QString mimeType) const
{
    DocumentClass documentClass = UnknownDocument;
    if (mimeType == QLatin1String("application/vnd.oasis.opendocument.text")
            || mimeType == QLatin1String("application/msword")
            || mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.wordprocessingml.document")
            || mimeType == QLatin1String("application/vnd.openxmlformats-officedocument.wordprocessingml.template")
            || mimeType == QLatin1String("application/vnd.ms-word.document.macroEnabled.12")
            || mimeType == QLatin1String("application/vnd.ms-word.template.macroEnabled.12")
            || mimeType == QLatin1String("application/rtf")) {
        documentClass = TextDocument;
    } else if (mimeType == QLatin1String("text/plain")) {
        documentClass = PlainTextDocument;
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
               || mimeType == QLatin1String("application/vnd.ms-excel.template.macroEnabled.12") 
               || mimeType == QLatin1String("text/csv")) {
        documentClass = SpreadSheetDocument;
    } else if (mimeType == QLatin1String("application/pdf")) {
        documentClass = PDFDocument;
    } else if (mimeType == QLatin1String("image/vnd.djvu")
               || mimeType == QLatin1String("image/vnd.djvu+multipage")
               || mimeType == QLatin1String("image/x-djvu")
               || mimeType == QLatin1String("image/x.djvu")) {
        documentClass = DJVUDocument;
    }

    return documentClass;
}
