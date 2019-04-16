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

Rectangle {
    id: root

    property alias attachX: translationMove.from
    property point handle
    property size handleSize
    property bool dragged

    x: handle.x - width / 2
    y: handle.y - height / 2
    opacity: 0.5
    color: Theme.highlightDimmerColor
    width: Math.round(Theme.iconSizeSmall / 4) * 2 // ensure even number
    height: width
    radius: width / 2

    states: State {
        when: dragged
        name: "dragged"
        PropertyChanges {
            target: root
            width: handleSize.width > 0 ? handleSize.width : (Theme.paddingSmall / 2)
            height: handleSize.height > 0 ? handleSize.height : (Theme.paddingSmall / 2)
            radius: 0
        }
    }

    transitions: Transition {
        to: "dragged"
        reversible: true
        SequentialAnimation {
            NumberAnimation { property: "width"; duration: 100 }
            PropertyAction { property: "radius" }
            NumberAnimation { property: "height"; duration: 100 }
        }
    }

    ParallelAnimation {
        id: appearingMove
        FadeAnimation {
            target: root
            from: 0
            to: 0.5
        }
        NumberAnimation {
            id: translationMove
            duration: 200
            easing.type: Easing.InOutQuad
            target: root
            property: "x"
            to: root.x
        }
    }

    onVisibleChanged: {
        if (visible) {
            appearingMove.start()
        }
    }
}
