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

import QtQuick 2.0
import Sailfish.Silica 1.0

Repeater {
    id: root

    property Item flickable
    property bool draggable: true
    property alias dragHandle1: handle1.dragged
    property alias dragHandle2: handle2.dragged

    visible: (model !== undefined && model.count > 0)

    delegate: Rectangle {
        opacity: 0.5
        color: Theme.highlightColor
        x: rect.x
        y: rect.y
        width: rect.width
        height: rect.height
    }

    children: [
        DJVUSelectionHandle {
            id: handle1
            visible: root.draggable
            attachX: root.flickable !== undefined
                     ? flickable.contentX
                     : handle.x - Theme.itemSizeExtraLarge
            handle: root.model.handle1
            dragHeight: root.model.handle1Height
        },
        DJVUSelectionHandle {
            id: handle2
            visible: root.draggable
            attachX: root.flickable !== undefined
                     ? flickable.contentX + flickable.width
                     : handle.x + Theme.itemSizeExtraLarge
            handle: root.model.handle2
            dragHeight: root.model.handle2Height
        }
    ]
}
