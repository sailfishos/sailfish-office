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

#include "pdfsearchmodel.h"

#include <QRectF>

class PDFSearchModel::Private
{
public:
    Private()
      : m_fraction(0.f)
    {
        roles.insert(Page, "page");
        roles.insert(Rect, "rect");
    }
    QList<QPair<int, QRectF> > m_matches;
    QHash<int, QByteArray> roles;
    float m_fraction;
};

PDFSearchModel::PDFSearchModel(QObject *parent)
  : QAbstractListModel(parent), d(new Private)
{
}

PDFSearchModel::~PDFSearchModel()
{
}

QHash<int, QByteArray> PDFSearchModel::roleNames() const
{
    return d->roles;
}

QVariant PDFSearchModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if (index.isValid()) {
        int row = index.row();
        if (row > -1 && row < d->m_matches.count()) {
            const QPair<int, QRectF> &match = d->m_matches.at(row);
            switch(role)
            {
            case Page:
                result.setValue<int>(match.first);
                break;
            case Rect:
                result.setValue<QRectF>(match.second);
                break;
            default:
                result.setValue<QString>(QString("Unknown role: %1").arg(role));
                break;
            }
        }
    }
    return result;
}

int PDFSearchModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return d->m_matches.count();
}

int PDFSearchModel::count() const
{
    return d->m_matches.count();
}

float PDFSearchModel::fraction() const
{
    return d->m_fraction;
}

void PDFSearchModel::addMatches(float fraction, const QList<QPair<int, QRectF> > &matches)
{
    if (!matches.empty()) {
        beginInsertRows(QModelIndex(), d->m_matches.count(),
                        d->m_matches.count() + matches.count() - 1);
        d->m_matches.append(matches);
        endInsertRows();
        emit countChanged();
    }

    d->m_fraction = fraction;
    emit fractionChanged();
}
