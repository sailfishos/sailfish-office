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

#ifndef SAILFISHAPPLICATION_H
#define SAILFISHAPPLICATION_H

#include <QSharedPointer>

class QString;
class QApplication;
class QQuickView;

namespace Sailfish 
{
    QSharedPointer< QApplication > createApplication(int& argc, char** argv);
    QSharedPointer< QQuickView > createView(const QString& file);
    void showView(const QSharedPointer<QQuickView> &view);
}

#endif // SAILFISHAPPLICATION_H

