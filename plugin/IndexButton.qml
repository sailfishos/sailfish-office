/*
 * Copyright (C) 2019 Jolla Ltd.
 * Contact: Joona Petrell <joona.petrell@jolla.com>
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

MouseArea {
    id: root

    property bool allowed: true
    property color color: Theme.primaryColor
    property int index
    property int count
    readonly property bool highlighted: pressed && containsMouse

    enabled: count > 1 && allowed
    opacity: count > 0 && allowed ? (count > 1 ? 1.0 : Theme.opacityHigh) : 0.0
    width: Math.min(Theme.itemSizeMedium, label.implicitWidth + Theme.paddingSmall)
    height: parent.height

    Label {
        id: label

        anchors.centerIn: parent
        width: parent.width - Theme.paddingSmall
        fontSizeMode: Text.HorizontalFit
        color: root.highlighted ? Theme.highlightColor : parent.color
        text: index + " | " + count
    }
}
