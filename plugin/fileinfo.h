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

#ifndef FILEINFO_H
#define FILEINFO_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QDateTime>

class FileInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY sourceChanged)
    Q_PROPERTY(QUrl fullPath READ fullPath NOTIFY sourceChanged)
    Q_PROPERTY(qint64 fileSize READ fileSize NOTIFY sourceChanged)
    Q_PROPERTY(QString mimeType READ mimeType NOTIFY sourceChanged)
    Q_PROPERTY(QString mimeTypeComment READ mimeTypeComment NOTIFY sourceChanged)
    Q_PROPERTY(QDateTime modifiedDate READ modifiedDate NOTIFY sourceChanged)

public:
    explicit FileInfo(QObject *parent = 0);
    ~FileInfo();

    QString source() const;
    QString fileName() const;
    QUrl fullPath() const;
    qint64 fileSize() const;
    QString mimeType() const;
    QString mimeTypeComment() const;
    QDateTime modifiedDate() const;

public Q_SLOTS:
    void setSource(const QString &source);

Q_SIGNALS:
    void sourceChanged();

private:
    class Private;
    Private* const d;
};

#endif // FILEINFO_H
