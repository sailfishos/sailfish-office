/*
 *
 */

#include "FileInfo.h"

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

FileInfo::FileInfo(QObject* parent)
    : QObject(parent), d(new Private)
{

}

FileInfo::~FileInfo()
{
}

QString FileInfo::source() const
{
    return d->source;
}

void FileInfo::setSource(const QString& source)
{
    if(source != d->source) {
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
    if(path.isEmpty())
        path = QUrl::fromLocalFile(source);

    if(path.isRelative())
        path = QUrl::fromLocalFile(QDir::current().absoluteFilePath(source));

    fileInfo = QFileInfo(path.toLocalFile());
    QMimeDatabase db;
    mimeType = db.mimeTypeForFile(fileInfo);
}
