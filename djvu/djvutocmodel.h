/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Copyright (C) 2022 Yura Beznos <yura.beznos@you-ra.info>
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

#ifndef DJVUTOCMODEL_H
#define DJVUTOCMODEL_H

#include <QtCore/QAbstractListModel>
#include "djvu.h"
//namespace Poppler { class Document; }

class DJVUTocModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

public:
    enum DJVUTocModelRoles {
        Title = Qt::UserRole + 1,
        Level,
        PageNumber
    };
    explicit DJVUTocModel(KDjVu *document, QObject *parent = 0);
    virtual ~DJVUTocModel();

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QHash<int, QByteArray> roleNames() const;

    int count() const;
    bool ready() const;

    Q_INVOKABLE void requestToc();

Q_SIGNALS:
    void countChanged();
    void readyChanged();

private Q_SLOTS:
    void onTocAvailable();

private:
    class Private;
    Private * const d;
};

#endif // DJVUTOCMODEL_H
