/*
 * Copyright (C) 2019 Open Mobile Platform LLC
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
    setSortParameter(SortParameter::Date);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

FilterModel::~FilterModel()
{
}

void FilterModel::setSourceModel(DocumentListModel *model)
{
    QSortFilterProxyModel::setSourceModel(static_cast<QAbstractItemModel*>(model));
}

int FilterModel::sortParameter() const
{
    return m_sortParameter;
}

void FilterModel::setSortParameter(int sortParameter)
{
    if (m_sortParameter == sortParameter) {
        return;
    }
    
    m_sortParameter = sortParameter;
    Qt::SortOrder order = Qt::AscendingOrder;

    switch (m_sortParameter) {
    case Name:
        setSortRole(DocumentListModel::Roles::FileNameRole);
        break;
    case Type:
        setSortRole(DocumentListModel::Roles::FileTypeAndNameRole);
        break;
    case Date:
        setSortRole(DocumentListModel::Roles::FileDateRole);
        order = Qt::DescendingOrder;
        break;
    default:
        break;
    }

    emit sortParameterChanged();
    
    sort(0, order);
}

DocumentListModel* FilterModel::sourceModel() const
{
    return static_cast<DocumentListModel*>(QSortFilterProxyModel::sourceModel());
}
