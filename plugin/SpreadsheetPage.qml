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

    onStatusChanged: {
        //Reset the position when we change sheets
        if (status === PageStatus.Activating) {
            flickable.contentX = 0
            flickable.contentY = 0
        }
    }

    icon: "image://theme/icon-m-file-spreadsheet"
    backgroundColor: "white"

    document.onStatusChanged: {
        if (document.status === Calligra.DocumentStatus.Loaded) {
            viewController.zoomToFitWidth(page.width)
        }
    }

    Calligra.View {
        id: documentView

        property bool contentAvailable: !page.busy

        anchors.fill: parent
        document: page.document
    }

    ControllerFlickable {
        id: flickable

        onZoomedChanged: overlay.active = !zoomed

        controller: viewController
        anchors.fill: parent
        enabled: !page.busy
        opacity: enabled ? 1.0 : 0.0
        Behavior on opacity { FadeAnimator { duration: 400 }}

        Calligra.ViewController {
            id: viewController
            view: documentView
            flickable: flickable
            useZoomProxy: false
            maximumZoom: Math.max(10.0, 2.0 * minimumZoom)
            minimumZoomFitsWidth: true
        }

        Calligra.LinkArea {
            anchors.fill: parent
            document: page.document
            onClicked: {
                if (flickable.zoomed) {
                    flickable.zoomOut()
                } else {
                    overlay.active = !overlay.active
                }
            }
            onLinkClicked: Qt.openUrlExternally(linkTarget)
            controllerZoom: viewController.zoom
        }
    }

    Item {
        id: overlay
        property bool active: true

        enabled: active
        anchors.fill: parent
        opacity: enabled ? 1.0 : 0.0
        Behavior on opacity { FadeAnimator {}}

        FadeGradient {
            topDown: true
            width: parent.width
            height: header.height + Theme.paddingLarge
            color: page.backgroundColor
        }

        DocumentHeader {
            id: header
            detailsPage: "SpreadsheetDetailsPage.qml"
            color: Theme.darkPrimaryColor
            page: page
        }

        OverlayToolbar {
            enabled: page.document.status === Calligra.DocumentStatus.Loaded
            opacity: enabled ? 1.0 : 0.0
            color: page.backgroundColor
            Behavior on opacity { FadeAnimator { duration: 400 }}

            DeleteButton {
                page: page
                icon.color: Theme.darkPrimaryColor
            }

            ShareButton {
                page: page
                icon.color: Theme.darkPrimaryColor
            }

            IndexButton {
                onClicked: pageStack.animatorPush(Qt.resolvedUrl("SpreadsheetListPage.qml"), { document: page.document })
                index: Math.max(1, page.document.currentIndex + 1)
                count: page.document.indexCount
                color: Theme.darkPrimaryColor
            }
        }
    }
}
