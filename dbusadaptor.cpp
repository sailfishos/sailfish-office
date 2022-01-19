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

#include "dbusadaptor.h"

#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

DBusAdaptor::DBusAdaptor(QQuickView *view)
    : QDBusAbstractAdaptor(view)
    , m_view(view)
{
}

DBusAdaptor::~DBusAdaptor()
{
}

void DBusAdaptor::Open(const QStringList &uris, const QVariantMap &platformData)
{
    if (!uris.isEmpty()) {
        QMetaObject::invokeMethod(m_view->rootObject(), "openFile", Q_ARG(QVariant, uris.at(0)));
    } else {
        Activate(platformData);
    }
}

void DBusAdaptor::Activate(const QVariantMap &platformData)
{
    Q_UNUSED(platformData)
    QMetaObject::invokeMethod(m_view->rootObject(), "activate");
}
