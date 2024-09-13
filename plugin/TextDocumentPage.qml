/*
 * Copyright (c) 2013 - 2019 Jolla Ltd.
 * Copyright (c) 2019 Open Mobile Platform LLC.
 *
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
import Sailfish.Silica.private 1.0
import org.kde.calligra 1.0 as Calligra

CalligraDocumentPage {
    id: page

    icon: "image://theme/icon-m-file-formatted"

    function currentIndex() {
        // Text document indexes appear to start at 1, model indexes at the traditional 0.
        return document.currentIndex - 1
    }

    document.onStatusChanged: {
        if (document.status === Calligra.DocumentStatus.Loaded) {
            viewController.zoomToFitWidth(page.width)
        }
    }

    Calligra.View {
        id: documentView

        property bool contentAvailable: !page.busy

        anchors.fill: flickable
        opacity: page.busy ? 0.0 : 1.0
        Behavior on opacity { FadeAnimator { duration: 400 }}
        document: page.document
    }

    ControllerFlickable {
        id: flickable

        property bool resetPositionWorkaround

        onContentYChanged: {
            if (page.document.status == Calligra.DocumentStatus.Loaded
                    && !resetPositionWorkaround) {
                // Calligra is not Flickable.topMargin aware
                contentY = -topMargin
                contentX = 0
                viewController.useZoomProxy = false
                resetPositionWorkaround = true
            }
        }

        controller: viewController
        topMargin: header.height
        clip: anchors.bottomMargin > 0
        anchors {
            fill: parent
            bottomMargin: toolbar.offset
        }


        Calligra.ViewController {
            id: viewController
            view: documentView
            flickable: flickable
            maximumZoom: Math.max(5.0, 2.0 * minimumZoom)
            minimumZoomFitsWidth: true
        }

        Calligra.LinkArea {
            anchors.fill: parent
            document: page.document
            onLinkClicked: Qt.openUrlExternally(linkTarget)
            onClicked: flickable.zoomOut()

            controllerZoom: viewController.zoom
        }

        DocumentHeader {
            id: header
            detailsPage: "TextDetailsPage.qml"
            page: page
            width: page.width
            x: flickable.contentX
            y: -height
        }
    }

    ToolBar {
        id: toolbar

        flickable: flickable
        anchors.top: flickable.bottom
        forceHidden: page.document.status === Calligra.DocumentStatus.Failed
        enabled: page.document.status === Calligra.DocumentStatus.Loaded
        opacity: enabled ? 1.0 : 0.0
        Behavior on opacity { FadeAnimator { duration: 400 }}

        DeleteButton {
            page: page
        }

        ShareButton {
            page: page
        }

        IndexButton {
            onClicked: pageStack.animatorPush(Qt.resolvedUrl("TextDocumentToCPage.qml"),
                                              { document: page.document, contents: page.contents })

            index: Math.max(1, page.document.currentIndex)
            count: page.document.indexCount
            allowed: page.document.status !== Calligra.DocumentStatus.Failed
        }
    }
}
