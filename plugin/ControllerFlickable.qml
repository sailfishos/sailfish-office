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

DocumentFlickable {
    id: flickable

    property QtObject controller

    pinchArea.onPinchUpdated: {
        var oldWidth = contentWidth
        var oldHeight = contentHeight
        var oldZoom = controller.zoom
        controller.zoomAroundPoint(controller.zoom * (pinch.scale - pinch.previousScale), 0, 0)

        if (controller.zoom === oldZoom) return

        var multiplier = (1.0 + pinch.scale - pinch.previousScale)
        var newWidth = multiplier * oldWidth
        var newHeight = multiplier * oldHeight

        contentX += pinch.previousCenter.x - pinch.center.x
        contentY += pinch.previousCenter.y - pinch.center.y

        // zoom about center
        if (newWidth > width)
            contentX -= (oldWidth - newWidth)/(oldWidth/pinch.previousCenter.x)
        if (newHeight > height)
            contentY -= (oldHeight - newHeight)/(oldHeight/pinch.previousCenter.y)
    }

    function zoomOut() {
        var scale = controller.zoom / controller.minimumZoom
        zoomOutContentYAnimation.to = Math.max(-topMargin,
                                               Math.min(flickable.contentHeight - flickable.height,
                                                        (flickable.contentY + flickable.height/2) / scale - flickable.height/2))
        zoomOutAnimation.start()
    }

    ParallelAnimation {
        id: zoomOutAnimation

        onStopped: flickable.returnToBounds()
        NumberAnimation {
            target: controller
            property: "zoom"
            to: controller.minimumZoom
            easing.type: Easing.InOutQuad
            duration: 200
        }
        NumberAnimation {
            target: flickable
            properties: "contentX"
            to: 0
            easing.type: Easing.InOutQuad
            duration: 200
        }
        NumberAnimation {
            id: zoomOutContentYAnimation
            target: flickable
            properties: "contentY"
            easing.type: Easing.InOutQuad
            duration: 200
        }
    }
}
