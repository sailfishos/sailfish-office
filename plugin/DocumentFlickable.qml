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
import Sailfish.Silica.private 1.0

SilicaFlickable {
    id: flickable

    readonly property bool zoomed: contentWidth > width
    property alias pinchArea: pinchArea
    default property alias foreground: pinchArea.data

    // Make sure that _noGrabbing will be reset back to false (JB#42531)
    Component.onDestruction: if (!visible) pageStack._noGrabbing = false

    // Override SilicaFlickable's pressDelay because otherwise it will
    // block touch events going to PinchArea in certain cases.
    pressDelay: 0
    interactive: !dragDetector.horizontalDragUnused
    ScrollDecorator { color: Theme.highlightDimmerColor }

    Binding { // Allow page navigation when panning the document near the top or bottom edge
        target: pageStack
        when: flickable.visible
        property: "_noGrabbing"
        value: dragDetector.horizontalDragUnused
    }

    Connections {
        target: pageStack
        onDragInProgressChanged: {
            if (pageStack.dragInProgress && pageStack._noGrabbing) {
                pageStack._grabMouse()
            }
        }
    }

    DragDetectorItem {
        id: dragDetector
        flickable: flickable
        anchors.fill: parent
        PinchArea {
            id: pinchArea

            onPinchFinished: flickable.returnToBounds()
            anchors.fill: parent
            enabled: !pageStack.dragInProgress
        }
    }
}
