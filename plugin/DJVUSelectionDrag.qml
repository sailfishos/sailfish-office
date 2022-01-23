/*
 * Copyright (C) 2016 Caliste Damien
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

import QtQuick 2.0
import Sailfish.Silica 1.0

MouseArea {
    id: root

    property point handle
    property Item flickable

    property real _contentY0
    property real _contentY: flickable ? flickable.contentY : 0.0
    property real _dragX0
    property real _dragY0
    property real dragX
    property real dragY

    signal dragged(point at)

    function reset() {
        dragX = 0
        dragY = 0
    }

    width: Theme.itemSizeSmall
    height: width

    enabled: visible
    preventStealing: true
    onPressed: {
        _dragX0 = mouseX
        _dragY0 = mouseY
        _contentY0 = (flickable ? flickable.contentY : 0.0)
    }
    onCanceled: reset()
    onReleased: reset()

    Binding {
        target: root
        property: "x"
        value: root.handle.x - root.width / 2
        when: !root.pressed
    }
    Binding {
        target: root
        property: "y"
        value: root.handle.y - root.height / 2
        when: !root.pressed
    }
    onMouseXChanged: dragX = pressed ? mouseX - _dragX0 : 0.
    onMouseYChanged: dragY = pressed ? mouseY - _dragY0 - _contentY + _contentY0 : 0.
    onDragXChanged: {
        if (pressed) {
            root.dragged(Qt.point(x + width / 2 + dragX, y + height / 2 + dragY))
        }
    }
    onDragYChanged: {
        if (pressed) {
            root.dragged(Qt.point(x + width / 2 + dragX, y + height / 2 + dragY))
        }
    }

    Rectangle {
        x: (root.width - width) / 2 + dragX
        y: (root.height - height) / 2 + dragY
        width: Theme.iconSizeSmall / 2 * 1.414
        height: width
        visible: opacity > 0.
        opacity: root.pressed ? 0.25 : 0.
        Behavior on opacity { FadeAnimator {} }
        radius: width / 2
        color: Qt.rgba(1. - Theme.highlightDimmerColor.r,
                       1. - Theme.highlightDimmerColor.g,
                       1. - Theme.highlightDimmerColor.b,
                       1.)
        Rectangle {
            anchors.centerIn: parent
            color: Theme.highlightDimmerColor
            width: Theme.iconSizeSmall / 2
            height: width
            radius: width / 2
        }
    }
}
