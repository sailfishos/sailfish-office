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
    property int indexCount
    property DocumentPage page
    property color color: Theme.primaryColor
    readonly property bool down: pressed && containsMouse

    onClicked: pageStack.animatorPush(Qt.resolvedUrl("DetailsPage.qml"), {
                                          source: page.source,
                                          indexCount: indexCount,
                                          mimeType: page.mimeType
                                      })

    width: parent.width
    height: pageHeader.height
    enabled: !page.busy && !page.error

    PageHeader {
        id: pageHeader
        title: page.title
        titleColor: parent.down ? Theme.highlightColor : parent.color
        rightMargin: Theme.horizontalPageMargin + detailsImage.width + Theme.paddingMedium
    }

    HighlightImage {
        id: detailsImage
        color: parent.color
        source: "image://theme/icon-m-about"
        highlighted: parent.down
        Behavior on opacity { FadeAnimator {}}
        opacity: parent.enabled ? 1.0 : Theme.opacityHigh

        anchors  {
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }
    }
}
