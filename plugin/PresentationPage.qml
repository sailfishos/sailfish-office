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
import Sailfish.Silica.private 1.0
import org.kde.calligra 1.0 as Calligra

DocumentPage {
    id: page

    busy: doc.status != Calligra.DocumentStatus.Loaded
          && doc.status != Calligra.DocumentStatus.Failed
    documentItem: view
    source: doc.source

    FadeBlocker {}

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if (status == PageStatus.Active) {
            doc.source = page.source
        }
    }

    SlideshowView {
        id: view

        property bool contentAvailable: !page.busy

        anchors.fill: parent
        orientation: Qt.Vertical
        currentIndex: doc.currentIndex

        enabled: !page.busy
        opacity: enabled ? 1.0 : 0.0
        Behavior on opacity { FadeAnimator { duration: 400 }}

        model: Calligra.ContentsModel {
            id: contentsModel
            document: doc
            thumbnailSize.width: page.width
            thumbnailSize.height: page.width * 0.75
        }

        delegate: ZoomableFlickable {
            id: flickable

            readonly property bool active: PathView.isCurrentItem || viewMoving
            onActiveChanged: {
                if (!active) {
                    resetZoom()
                    largeThumb.data = contentsModel.thumbnail(-1, 0)
                }
            }

            onZoomedChanged: overlay.active = !zoomed
            onZoomFinished: if (largeThumb.implicitWidth === 0) largeThumb.data = contentsModel.thumbnail(model.index, 3264)

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
                        data = contentsModel.thumbnail(model.index, Screen.height)
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

        enabled: active && !deleteButton.remorseActive
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
            color: Theme.lightPrimaryColor
            page: page
            indexCount: doc.indexCount
        }

        OverlayToolbar {
            enabled: doc.status == Calligra.DocumentStatus.Loaded
            opacity: enabled ? 1.0 : 0.0
            Behavior on opacity { FadeAnimator { duration: 400 }}

            DeleteButton {
                id: deleteButton
                page: page
                icon.color: Theme.lightPrimaryColor
            }

            ShareButton {
                page: page
                icon.color: Theme.lightPrimaryColor
            }

            IndexButton {
                onClicked: pageStack.animatorPush(Qt.resolvedUrl("PresentationThumbnailPage.qml"), { document: doc })

                index: Math.max(1, view.currentIndex + 1)
                count: doc.indexCount
                color: Theme.lightPrimaryColor
            }
        }
    }

    Calligra.Document {
        id: doc
        onStatusChanged: {
            if (status == Calligra.DocumentStatus.Failed) {
                errorLoader.setSource(Qt.resolvedUrl("FullscreenError.qml"), { error: lastError })
            }
        }

    }

    Loader {
        id: errorLoader
        anchors.fill: parent
    }
}
