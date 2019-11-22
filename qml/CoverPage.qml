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

import QtQuick 2.4
import Sailfish.Silica 1.0

CoverBackground {
    id: root

    // While peeking both main window and cover window are visible at the
    // same time. Thus, we can capture from main window when cover
    // becomes visible.
    // Note: this may change in future.
    onVisibleChanged: {
        if (visible) {
            previewImage.updatePreview()
        } else if (window.visible) {
            previewImage.source = ""
        }
    }

    CoverPlaceholder {
        //: Cover placeholder shown when there are no documents
        //% "No documents"
        text: qsTrId("sailfish-office-la-cover_no_documents")
        icon.source: "image://theme/icon-launcher-office"
        visible: window.documentItem === null && fileListView.count == 0
    }

    property int iconSize: Math.round(Theme.iconSizeMedium * 0.8)

    ListView {
        id: fileListView

        property int itemHeight: height/maxItemCount
        property int maxItemCount: Math.round(height/(Math.max(fontMetrics.height, iconSize) + Theme.paddingSmall))
        clip: true
        interactive: false
        model: window.fileListModel
        visible: window.documentItem === null
        anchors {
            fill: parent
            topMargin: Theme.paddingLarge
            bottomMargin: Theme.paddingLarge
        }

        delegate: CoverFileItem {
            width: fileListView.width
            height: fileListView.itemHeight
            text: model.fileName
            iconSource: window.mimeToIcon(model.fileMimeType)
            iconSize: root.iconSize
        }
        FontMetrics {
            id: fontMetrics
            font.pixelSize: Theme.fontSizeMedium
        }
    }

    Item {
        property bool isPortrait: !pageStack.currentPage || pageStack.currentPage.isPortrait

        anchors.centerIn: parent
        width: isPortrait ? parent.width : parent.height
        height: isPortrait ? parent.height : parent.width
        rotation: isPortrait ? 0 : 90
        visible: window.documentItem != null

        Image {
            id: previewImage

            anchors.fill: parent
            visible: window.documentItem && (!window.documentItem.hasOwnProperty("contentAvailable") ||
                                              window.documentItem.contentAvailable)

            function updatePreview() {
                if (window.visible && window.documentItem) {
                    window.documentItem.grabToImage(function(result) {
                        previewImage.source = result.url
                    }, Qt.size(width, height))
                }
            }
            Connections {
                target: window
                onDocumentItemChanged: delayedUpdate.restart()
                onOrientationChanged: delayedUpdate.restart()
            }
            Timer {
                id: delayedUpdate
                interval: 16
                onTriggered: previewImage.updatePreview()
            }
        }

        // fall back to file name if content is not loaded
        CoverFileItem {
            visible: !previewImage.visible
            width: parent.width
            y: Theme.paddingLarge
            multiLine: true
            text: window.documentItem && window.documentItem.hasOwnProperty("title")
                  ? window.documentItem.title : ""
            iconSource: window.documentItem && window.documentItem.hasOwnProperty("mimeType")
                        ? window.mimeToIcon(window.documentItem.mimeType) : ""
        }
    }
}
