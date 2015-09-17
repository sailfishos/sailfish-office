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

#include "taglistmodel.h"

struct TagListModelEntry
{
    QString label;
    unsigned int index;
    unsigned int usage;
};

class TagListModel::Private
{
public:
    Private() {
        roles.insert(TagLabelRole, "label");
        roles.insert(TagUsageRole, "usage");
    }
    QList<TagListModelEntry> tags;
    QHash<int, QByteArray> roles;
};

TagListModel::TagListModel(QObject* parent)
    : QAbstractListModel(parent), d(new Private)
{
}

TagListModel::~TagListModel()
{
}

QVariant TagListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= d->tags.count())
        return QVariant();
    
    switch (role) {
    case TagLabelRole:
        return d->tags.at(index.row()).label;
    case TagUsageRole:
        return d->tags.at(index.row()).usage;
    default:
        break;
    }

    return QVariant();
}

int TagListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return d->tags.count();
}
int TagListModel::count() const
{
  return d->tags.count();
}

QHash< int, QByteArray > TagListModel::roleNames() const
{
    return d->roles;
}

bool TagListModel::contains(const QString &tag) const
{
    for (QList<TagListModelEntry>::const_iterator entry = d->tags.begin();
         entry != d->tags.end(); entry++) {
        if (entry->label == tag)
            return true;
    }
    return false;
}

void TagListModel::addItem(const QString &tag)
{
    for (QList<TagListModelEntry>::iterator entry = d->tags.begin();
         entry != d->tags.end(); entry++) {
        if (QString::localeAwareCompare(entry->label, tag) == 0) {
            entry->usage += 1;
            dataChanged(index(entry->index), index(entry->index));
            return;
        }
    }

    TagListModelEntry entry;
    entry.label = tag;
    entry.usage = 1;
    // Add the new tag in the alphabetic order.
    for (entry.index = 0; entry.index < d->tags.count(); entry.index++)
        if (QString::localeAwareCompare(d->tags.at(entry.index).label, tag) > 0)
            break;
    beginInsertRows(QModelIndex(), entry.index, entry.index);
    d->tags.insert(entry.index, entry);
    endInsertRows();
    emit countChanged();
}

void TagListModel::removeItem(const QString &tag)
{
    for(QList<TagListModelEntry>::iterator entry = d->tags.begin();
        entry != d->tags.end(); entry++) {
        if (entry->label == tag) {
            entry->usage -= 1;
            if (entry->usage) {
                dataChanged(index(entry->index), index(entry->index));
            } else {
                beginRemoveRows(QModelIndex(), entry->index, entry->index);
                d->tags.removeAt(entry->index);
                endRemoveRows();
                emit countChanged();
            }
            return;
        }
    }
}
