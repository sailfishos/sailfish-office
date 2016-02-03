/*
 * Copyright (C) 2016 Caliste Damien.
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

import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: root

    property bool press
    property point handle
    property alias attachX: appearingMove.from

    signal dragged(point at)

    width: Theme.itemSizeSmall
    height: width

    NumberAnimation {
        id: appearingMove
        duration: 200
        easing.type: Easing.InOutCubic
        target: root
        property: "x"
        to: root.handle.x - root.width / 2
    }
    onVisibleChanged: {
        if (visible) {
            appearingMove.start()
        }
    }

    Binding {
        target: root
        property: "x"
        value: root.handle.x - root.width / 2
        when: !mouseArea.drag.active
    }
    Binding {
        target: root
        property: "y"
        value: root.handle.y - root.height / 2
        when: !mouseArea.drag.active
    }
    onXChanged: {
        if (mouseArea.drag.active) {
            root.dragged(Qt.point(x + width / 2, y + height / 2))
        }
    }
    onYChanged: {
        if (mouseArea.drag.active) {
            root.dragged(Qt.point(x + width / 2, y + height / 2))
        }
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.visible
        preventStealing: true
        onPressed: root.press = true
        onReleased: root.press = false
        drag.target: parent
    }
    Rectangle {
        anchors.centerIn: parent
        opacity: 0.5
        color: Theme.highlightDimmerColor
        width: Theme.paddingMedium
        height: width
        radius: width / 2
    }
}
