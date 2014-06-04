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

#ifndef SAILFISHOFFICEPLUGIN_H
#define SAILFISHOFFICEPLUGIN_H

#include <QQmlExtensionPlugin>
#include <QQmlEngine>

class SailfishOfficePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Sailfish.Office")
public:
    explicit SailfishOfficePlugin(QObject* parent = 0);

    virtual void registerTypes(const char* uri);
    virtual void initializeEngine(QQmlEngine* engine, const char* uri);
};

#endif // SAILFISHOFFICEPLUGIN_H
