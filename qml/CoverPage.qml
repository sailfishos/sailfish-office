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

CoverBackground {
    CoverPlaceholder {
        //: Cover placeholder shown when there are no documents
        //% "No documents"
        text: qsTrId("sailfish-office-la-cover_no_documents")
        icon.source: "image://theme/icon-launcher-office"
        visible: window.documentItem === null && fileListView.count == 0
    }

    ListView {
        id: fileListView

        property real itemHeight: Theme.iconSizeSmall + Theme.paddingSmall*2

        clip: true
        interactive: false
        model: window.fileListModel
        visible: window.documentItem === null
        y: Theme.paddingLarge
        width: parent.width
        height: 7*itemHeight

        delegate: Item {
            width: fileListView.width
            height: fileListView.itemHeight
            Image {
                id: icon

                property string fileMimeType: window.mimeToIcon(model.fileMimeType)

                anchors {
                    left: parent.left
                    leftMargin: Theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
                source: fileMimeType !== "" ? fileMimeType
                                            : "image://theme/icon-l-document"

                sourceSize { width: Theme.iconSizeSmall; height: Theme.iconSizeSmall }
            }
            Label {
                anchors {
                    left: icon.right
                    leftMargin: Theme.paddingMedium
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    rightMargin: Theme.paddingLarge
                }
                text: model.fileName
                truncationMode: TruncationMode.Fade
            }
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
            property QtObject coverWindow

            function updatePreview() {
                if (window.documentItem && applicationWindow.visible) {
                    window.documentItem.grabToImage(function(result) { previewImage.source = result.url },
                                                             Qt.size(width, height))
                }
            }
            Component.onCompleted: {
                coverWindow = coverWindowAccessor.coverWindow()
            }
            Connections {
                target: previewImage.coverWindow
                onVisibilityChanged: previewImage.updatePreview()
                ignoreUnknownSignals: true
            }
            Connections {
                target: window
                onDocumentItemChanged: { previewImage.updatePreview() }
                onOrientationChanged: { previewImage.updatePreview() }
            }
        }
    }
}
