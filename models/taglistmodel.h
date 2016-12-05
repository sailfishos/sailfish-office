/*
 * Copyright (C) 2015 Damien Caliste
 * Contact: Damien Caliste <dcaliste@free.fr>
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

#ifndef TAGLISTMODEL_H
#define TAGLISTMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

class TagListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles
    {
        TagLabelRole = Qt::UserRole + 1,
        TagUsageRole
    };

    TagListModel( QObject* parent = 0 );
    ~TagListModel();

    TagListModel( const TagListModel& ) = delete;
    TagListModel& operator=( const TagListModel& ) = delete;

    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent) const;
    virtual QHash< int, QByteArray > roleNames() const;
    int count() const;

    Q_INVOKABLE bool contains(const QString &tag) const;
    void addItem(const QString &tag);
    void removeItem(const QString &tag);

Q_SIGNALS:
    void countChanged();

private:
    class Private;
    const QScopedPointer< Private > d;
};

class TagFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(TagListModel *sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
public:
    TagFilterModel(QObject* parent = 0);
    ~TagFilterModel();

public:
    TagListModel* sourceModel() const;

public Q_SLOTS:
    void setSourceModel(TagListModel *model);

Q_SIGNALS:
    void sourceModelChanged();
};

#endif // TAGLISTMODEL_H
