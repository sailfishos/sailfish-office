/*
 * Copyright (c) 2013 - 2022 Jolla Ltd.
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

#ifndef DBUSADAPTOR_H
#define DBUSADAPTOR_H

#include <QtDBus/QDBusAbstractAdaptor>
#include <QVariant>

class QQuickView;
class DBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Application")

public:
    DBusAdaptor(QQuickView *view);
    ~DBusAdaptor();

public Q_SLOTS:
    Q_NOREPLY void Open(const QStringList &uris, const QVariantMap &platformData);
    Q_NOREPLY void Activate(const QVariantMap &platformData);

private:
    QQuickView *m_view;
};

#endif // DBUSADAPTOR_H
