/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Contact: Robin Burchell <robin.burchell@jolla.com>
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
    : QDBusAbstractAdaptor(view), m_view{view}
{
}

DBusAdaptor::~DBusAdaptor()
{
}

void DBusAdaptor::openFile(const QStringList &files)
{
    if (files.count() > 0) {
        QMetaObject::invokeMethod(m_view->rootObject(), "openFile", Q_ARG(QVariant, files.at(0)));
    } else {
        QMetaObject::invokeMethod(m_view->rootObject(), "activate");
    }
}
