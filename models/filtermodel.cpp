/*
 * Copyright (C) 2015 Caliste Damien.
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

#include "filtermodel.h"

#include "documentlistmodel.h"

FilterModel::FilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    this->setFilterRole(DocumentListModel::Roles::FileNameRole);
}

FilterModel::~FilterModel()
{
}

void FilterModel::setSourceModel(DocumentListModel *model)
{
    QSortFilterProxyModel::setSourceModel(static_cast<QAbstractItemModel*>(model));
}

DocumentListModel* FilterModel::sourceModel() const
{
    return static_cast<DocumentListModel*>(QSortFilterProxyModel::sourceModel());
}

bool FilterModel::tagFiltered() const
{
    return !tags.empty();
}
bool FilterModel::hasTag(const QString &tag) const
{
    return tags.contains(tag);
}
void FilterModel::addTag(const QString &tag)
{
    tags.insert(tag);
    invalidateFilter();
    emit tagFilteringChanged();
}
void FilterModel::removeTag(const QString &tag)
{
    tags.remove(tag);
    invalidateFilter();
    emit tagFilteringChanged();
}
bool FilterModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
    bool ret;

    ret = true;
    for (QSet<QString>::const_iterator it = tags.begin();
         it != tags.end() && ret; it++) {
        ret = sourceModel()->hasTagAt(source_row, *it);
    }

    return ret && QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
