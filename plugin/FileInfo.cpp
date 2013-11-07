/*
 *
 */

#include "FileInfo.h"

#include <QtCore/QMimeType>
#include <QtCore/QFileInfo>
#include <QtCore/QMimeDatabase>

class FileInfo::Private
{
public:
    Private()
    { }

    QUrl source;

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

QUrl FileInfo::source() const
{
    return d->source;
}

void FileInfo::setSource(const QUrl& source)
{
    if(source != d->source) {
        d->source = source;

        d->fileInfo = QFileInfo(d->source.toLocalFile());
        QMimeDatabase db;
        d->mimeType = db.mimeTypeForFile(d->fileInfo);

        emit sourceChanged();
    }
}

QString FileInfo::fileName() const
{
    return d->fileInfo.fileName();
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
