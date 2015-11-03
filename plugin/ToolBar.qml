/*
 * Copyright (C) 2015 Caliste Damien.
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

PanelBackground {
    id: toolbar

    property var flickable: undefined
    property int parentHeight
    property bool hidden
    property bool autoHide: true

    property int _previousContentY
    property bool _dragUp

    anchors.top: flickable.bottom

    onAutoHideChanged: if (autoHide && !autoHideTimer.running) autoHideTimer.start()

    states: [
        State {
            name: "visible"
            when: !hidden && _dragUp
            PropertyChanges { target: flickable; height: parentHeight - toolbar.height }
        },
        State {
            name: "hidden"
            when: hidden || !_dragUp
            PropertyChanges { target: flickable; height: parentHeight }
        }
    ]
    transitions: Transition {
        NumberAnimation { target: flickable; property: "height"; duration: 400; easing.type: Easing.InOutQuad }
    }

    Binding {
        target: flickable
        property: "clip"
        value: enabled
    }
    Connections {
        target: flickable
        onContentYChanged:  {
        if (!flickable.movingVertically) return

        _dragUp = !autoHide || (flickable.contentY < _previousContentY)
        if (_dragUp) autoHideTimer.restart()

        _previousContentY = flickable.contentY
        }
    }

    Timer {
        id: autoHideTimer
        interval: 4000
        onTriggered: _dragUp = !autoHide
    }
}
