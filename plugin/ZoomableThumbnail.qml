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
import org.kde.calligra 1.0 as Calligra

SilicaFlickable {
    id: base

    property real maxHeight: height
    property alias content: thumb.data
    property bool scaled

    signal clicked()
    signal updateSize(real newWidth, real newHeight)

    height: Math.min(thumb.height, maxHeight)
    contentWidth: thumb.width
    contentHeight: thumb.height
    clip: true

    function zoom(amount, center) {
        var oldWidth = thumb.width
        var oldHeight = thumb.height

        thumb.width *= amount
        updateTimer.restart()

        if (thumb.width < d.minWidth) {
            thumb.width = d.minWidth
        }

        if (thumb.width > d.maxWidth) {
            thumb.width = d.maxWidth
        }

        if (Math.abs(thumb.width - d.minWidth) < 5 ) {
            base.scaled = false
        } else {
            base.scaled = true
        }

        var realZoom = thumb.width / oldWidth
        thumb.height *= realZoom

        contentX += (center.x * thumb.width / oldWidth) - center.x
        if (thumb.height > height) {
            contentY += (center.y * thumb.height / oldHeight) - center.y
        }
    }

    Calligra.ImageDataItem {
        id: thumb

        children: [
            PinchArea {
                anchors.fill: parent
                onPinchUpdated: base.zoom(1.0 + (pinch.scale - pinch.previousScale), pinch.center)
                onPinchFinished: base.returnToBounds()

                MouseArea {
                    anchors.fill: parent
                    onClicked: base.clicked()
                }
            }
        ]
    }

    QtObject {
        id: d

        property real minWidth: base.width
        property real maxWidth: base.width * 2.5
    }

    Timer {
        id: updateTimer

        interval: 500
        onTriggered: base.updateSize(thumb.width, thumb.height)
    }
}
