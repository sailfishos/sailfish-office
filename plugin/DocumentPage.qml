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

import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    property string title
    property url source
    property bool error
    property string mimeType
    property Item documentItem: page
    property alias busy: busyIndicator.running
    property QtObject provider
    property alias busyIndicator: busyIndicator

    Component.onDestruction: window.documentItem = null
    onStatusChanged: {
        if (status == PageStatus.Active) {
            window.documentItem = documentItem
        }
    }

    allowedOrientations: Orientation.All
    clip: status !== PageStatus.Active || pageStack.dragInProgress

    BusyIndicator { id: busyIndicator; anchors.centerIn: parent; size: BusyIndicatorSize.Large; z: 101 }
}
