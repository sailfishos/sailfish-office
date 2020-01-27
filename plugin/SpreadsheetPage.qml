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

DocumentPage {
    id: page

    onStatusChanged: {
        //Reset the position when we change sheets
        if (status == PageStatus.Activating) {
            flickable.contentX = 0
            flickable.contentY = 0
        }
    }

    busy: doc.status != Calligra.DocumentStatus.Loaded
          && doc.status != Calligra.DocumentStatus.Failed
    documentItem: documentView
    backNavigation: !busy // During loading the UI is unresponsive, don't show page indicator as back-stepping is not possible
    busyIndicator._forceAnimation: busy // Start animation before the main thread gets blocked by loading

    Timer {
        interval: 1
        running: status === PageStatus.Active
        // Delay loading the document until the page has been activated
        onTriggered: doc.source = page.source
    }

    FadeBlocker {
        id: fadeBlocker
        color: "white"
        Binding {
            when: fadeBlocker.fullscreen
            target: __silica_applicationwindow_instance.__quickWindow
            property: "color"
            value: Qt.application.active ? fadeBlocker.color : "transparent"
        }
    }

    Calligra.Document {
        id: doc
        readOnly: true
        onStatusChanged: {
            if (status == Calligra.DocumentStatus.Loaded) {
                viewController.zoomToFitWidth(page.width)
            } else if (status == Calligra.DocumentStatus.Failed) {
                errorLoader.setSource(Qt.resolvedUrl("FullscreenError.qml"), { error: lastError })
            }
        }
    }

    Calligra.View {
        id: documentView

        property bool contentAvailable: !page.busy

        anchors.fill: parent
        document: doc
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
            document: doc
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
            color: fadeBlocker.color
        }

        DocumentHeader {
            id: header
            color: Theme.darkPrimaryColor
            page: page
            indexCount: doc.indexCount
        }

        OverlayToolbar {
            enabled: doc.status == Calligra.DocumentStatus.Loaded
            opacity: enabled ? 1.0 : 0.0
            color: fadeBlocker.color
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
                onClicked: pageStack.animatorPush(Qt.resolvedUrl("SpreadsheetListPage.qml"), { document: doc })
                index: Math.max(1, doc.currentIndex + 1)
                count: doc.indexCount
                color: Theme.darkPrimaryColor
            }
        }
    }

    Loader {
        id: errorLoader
        anchors.fill: parent
    }
}
