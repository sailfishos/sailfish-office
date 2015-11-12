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

    property Item flickable
    property bool forceHidden
    property bool autoShowHide: true
    property int offset: _active && !forceHidden ? height : 0

    property bool _active
    property int _previousContentY

    onAutoShowHideChanged: {
        if (autoShowHide && _active) {
            autoHideTimer.start()
        } else {
            autoHideTimer.stop()
        }
    }

    Behavior on offset { NumberAnimation { duration: 400; easing.type: Easing.InOutQuad } }

    Connections {
        target: flickable
        onContentYChanged: {
            if (!flickable.movingVertically) {
                return
            }

            if (autoShowHide) {
                _active = flickable.contentY < _previousContentY

                if (_active) {
                    autoHideTimer.restart()
                }
            }

            _previousContentY = flickable.contentY
        }
    }

    Timer {
        id: autoHideTimer
        interval: 4000
        onTriggered: _active = false
    }
}
