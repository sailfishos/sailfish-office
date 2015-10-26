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

#include "fileinfo.h"

#include <QtCore/QMimeType>
#include <QtCore/QFileInfo>
#include <QtCore/QMimeDatabase>
#include <QtCore/QDir>

class FileInfo::Private
{
public:
    Private()
    { }

    void resolvePath();

    QString source;

    QUrl path;

    QFileInfo fileInfo;
    QMimeType mimeType;
};

FileInfo::FileInfo(QObject *parent)
    : QObject(parent), d(new Private)
{
}

FileInfo::~FileInfo()
{
    delete d;
}

QString FileInfo::source() const
{
    return d->source;
}

void FileInfo::setSource(const QString &source)
{
    if (source != d->source) {
        d->source = source;

        d->resolvePath();

        emit sourceChanged();
    }
}

QString FileInfo::fileName() const
{
    return d->fileInfo.fileName();
}

QUrl FileInfo::fullPath() const
{
    return d->path;
}

qint64 FileInfo::fileSize() const
{
    return d->fileInfo.size();
}

QString FileInfo::mimeType() const
{
    return d->mimeType.name();
}

QString FileInfo::mimeTypeComment() const
{
    return d->mimeType.comment();
}

QDateTime FileInfo::modifiedDate() const
{
    return d->fileInfo.lastModified();
}

void FileInfo::Private::resolvePath()
{
    path = QUrl(source);
    if (path.isEmpty())
        path = QUrl::fromLocalFile(source);

    if (path.isRelative())
        path = QUrl::fromLocalFile(QDir::current().absoluteFilePath(source));

    fileInfo = QFileInfo(path.toLocalFile());
    QMimeDatabase db;
    mimeType = db.mimeTypeForFile(fileInfo);
}
