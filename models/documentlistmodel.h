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

#ifndef DOCUMENTLISTMODEL_H
#define DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <qdatetime.h>

#include "tagsthread.h"
#include "trackertagprovider.h"
#include "taglistmodel.h"

class DocumentListModelPrivate;

class DocumentListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(TagListModel* tags READ tags NOTIFY tagsChanged)
public:
    enum DocumentClass {
        UnknownDocument,
        TextDocument,
        SpreadSheetDocument,
        PresentationDocument,
        PDFDocument
    };
    Q_ENUMS(DocumentClass)

    enum Roles
    {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        FileTypeRole,
        FileSizeRole,
        FileReadRole,
        FileMimeTypeRole,
        FileDocumentClass
    };

    DocumentListModel(QObject *parent = 0);
    ~DocumentListModel();

    DocumentListModel(const DocumentListModel&) = delete;
    DocumentListModel& operator=(const DocumentListModel&) = delete;

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QHash<int, QByteArray> roleNames() const;

    void setAllItemsDirty(bool status);
    void addItem(QString name, QString path, QString type, int size, QDateTime lastRead, QString mimeType);
    void removeItemsDirty();
    void removeAt(int index);
    void clear();

    TagListModel* tags() const;

    Q_INVOKABLE int mimeTypeToDocumentClass(QString mimeType) const;
    bool hasTag(int row, const QString &tag) const;
    Q_INVOKABLE bool hasTag(const QString &path, const QString &tag) const;
    Q_INVOKABLE void addTag(const QString &path, const QString &tag);
    Q_INVOKABLE void removeTag(const QString &path, const QString &tag);

private Q_SLOTS:
    void jobFinished(TagsThreadJob* job);
    void tagLoaded(const QString &path, const QList<QString> &tags);

Q_SIGNALS:
    void tagsChanged();

private:
    class Private;
    const QScopedPointer<Private> d;

    void notifyForPath(const QString &path);
};

#endif // DOCUMENTLISTMODEL_H
