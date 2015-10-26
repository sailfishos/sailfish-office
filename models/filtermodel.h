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

#ifndef FILTERMODEL_H
#define FILTERMODEL_H

#include <QSortFilterProxyModel>

#include "documentlistmodel.h"

class FilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(DocumentListModel *sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
public:
    FilterModel(QObject *parent = 0);
    ~FilterModel();

public:
    DocumentListModel* sourceModel() const;

public Q_SLOTS:
    void setSourceModel(DocumentListModel *model);

Q_SIGNALS:
    void sourceModelChanged();
};

#endif // FILTERMODEL_H
