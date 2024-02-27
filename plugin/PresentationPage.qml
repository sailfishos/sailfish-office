/*
 * Copyright (c) 2013-2020 Jolla Ltd.
 * Copyright (c) 2020 Open Mobile Platform LLC.
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

    icon: "image://theme/icon-m-file-presentation"
    backgroundColor: "black"
    coverAlignment: Qt.AlignCenter
    coverFillMode: Image.PreserveAspectFit
    busyIndicator.y: Math.round(page.height/2 - busyIndicator.height/2)

    function currentIndex() {
        return view.currentIndex >= 0 ? view.currentIndex : document.currentIndex
    }

    contents.thumbnailSize {
        width: page.width
        height: page.width * 0.75
    }

    SlideshowView {
        id: view

        property bool contentAvailable: !page.busy

        anchors.fill: parent
        orientation: Qt.Vertical
        currentIndex: page.document.currentIndex

        enabled: !page.busy
        opacity: enabled ? 1.0 : 0.0
        Behavior on opacity { FadeAnimator { duration: 400 }}

        model: page.contents

        delegate: ZoomableFlickable {
            id: flickable

            readonly property bool active: PathView.isCurrentItem || viewMoving
            onActiveChanged: {
                if (!active) {
                    resetZoom()
                    largeThumb.data = page.contents.thumbnail(-1, 0)
                }
            }

            onZoomedChanged: overlay.active = !zoomed
            onZoomFinished: if (largeThumb.implicitWidth === 0) largeThumb.data = page.contents.thumbnail(model.index, 3264)

            width: view.width
            height: view.height
            viewMoving: view.moving
            scrollDecoratorColor: Theme.highlightDimmerFromColor(Theme.highlightDimmerColor, Theme.DarkOnLight)
            implicitContentWidth: thumb.implicitWidth
            implicitContentHeight: thumb.implicitHeight

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (zoomed) {
                        zoomOut()
                    } else {
                        overlay.active = !overlay.active
                    }
                }
            }

            Calligra.ImageDataItem {
                id: thumb

                property bool initialized
                property bool ready: initialized && !viewMoving

                Component.onCompleted: initialized = true
                onReadyChanged: {
                    if (ready) {
                        ready = true // remove binding
                        data = page.contents.thumbnail(model.index, Screen.height)
                    }
                }

                anchors.fill: parent
            }
            Calligra.ImageDataItem {
                id: largeThumb
                visible: implicitWidth > 0
                anchors.fill: parent
            }
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
        }

        DocumentHeader {
            id: header
            detailsPage: "PresentationDetailsPage.qml"
            color: Theme.lightPrimaryColor
            page: page
        }

        OverlayToolbar {
            enabled: page.document.status == Calligra.DocumentStatus.Loaded
            opacity: enabled ? 1.0 : 0.0
            Behavior on opacity { FadeAnimator { duration: 400 }}

            DeleteButton {
                page: page
                icon.color: Theme.lightPrimaryColor
            }

            ShareButton {
                page: page
                icon.color: Theme.lightPrimaryColor
            }

            IndexButton {
                onClicked: pageStack.animatorPush(Qt.resolvedUrl("PresentationThumbnailPage.qml"), { document: page.document })

                index: Math.max(1, view.currentIndex + 1)
                count: page.document.indexCount
                color: Theme.lightPrimaryColor
            }
        }
    }
}
