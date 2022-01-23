/*
 * Copyright (C) 2016 Caliste Damien.
 * Copyright (C) 2022 Yura Beznos <yura.beznos@you-ra.info>
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

#ifndef DJVUSEARCHMODEL_H
#define DJVUSEARCHMODEL_H

#include <QtCore/QAbstractListModel>

class DJVUSearchModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(float fraction READ fraction NOTIFY fractionChanged)

public:
    enum DJVUSearchModelRoles {
        Page = Qt::UserRole + 1,
        Rect
    };
    explicit DJVUSearchModel(QObject *parent = 0);
    virtual ~DJVUSearchModel();

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QHash<int, QByteArray> roleNames() const;

    int count() const;
    float fraction() const;
    void addMatches(float fraction, const QList<QPair<int, QRectF> > &matches);

Q_SIGNALS:
    void countChanged();
    void fractionChanged();

private:
    class Private;
    const QScopedPointer<Private> d;
};

#endif // DJVUSEARCHMODEL_H
