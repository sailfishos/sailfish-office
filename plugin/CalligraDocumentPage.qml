/*
 * Copyright (c) 2013 - 2019 Jolla Ltd.
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

DocumentPage {
    id: page

    property alias document: doc
    property alias contents: contentsModel
    property alias backgroundColor: background.color
    property int coverAlignment: Qt.AlignLeft | Qt.AlignTop
    property int coverFillMode: Image.PreserveAspectCrop

    function currentIndex() {
        // This is a function which can be shadowed because the currentIndex property doesn't
        // notify reliably and nothing needs to bind to it.
        return doc.currentIndex
    }

    _opaqueBackground: background.color !== "transparent"

    Rectangle {
        id: background
        color: "transparent"
        z: -1
        anchors.fill: parent
        parent: page._backgroundParent
        visible: page._opaqueBackground
    }

    backNavigation: !busy // During loading the UI is unresponsive, don't show page indicator as back-stepping is not possible
    busyIndicator._forceAnimation: busy // Start animation before the main thread gets blocked by loading
    icon: "image://theme/icon-m-file-formatted"
    busy: doc.status !== Calligra.DocumentStatus.Loaded
            && doc.status !== Calligra.DocumentStatus.Failed

    Timer {
        interval: 1
        running: status === PageStatus.Active
        // Delay loading the document until the page has been activated
        onTriggered: document.source = page.source
    }

    Timer {
        id: previewDelay
        interval: 100
        running: doc.status === Calligra.DocumentStatus.Loaded
        // We're not using a binding for the preview because calligra is sensitive to the order
        // of evaluation and by binding directly to the document status it's possible to attempt
        // to get a thumbnail from the contents model after the document has loaded but before the
        // model is populated.
        onTriggered: page.preview = previewComponent
    }

    Component {
        id: previewComponent

        Rectangle {
            id: preview

            color: page.backgroundColor

            Calligra.ImageDataItem {
                x: {
                    if (page.coverAlignment & Qt.AlignHCenter) {
                        return (preview.width - width) / 2
                    } else if (page.coverAlignment & Qt.AlignRight) {
                        return preview.width - width
                    } else {
                        return 0
                    }
                }

                y: {
                    if (page.coverAlignment & Qt.AlignVCenter) {
                        return (preview.height - height) / 2
                    } else if (page.coverAlignment & Qt.AlignBottom) {
                        return preview.height - height
                    } else {
                        return 0
                    }
                }

                width: {
                    if (implicitHeight > 0 && page.coverFillMode === Image.PreserveAspectCrop) {
                        return Math.max(
                                    preview.width,
                                    Math.round(implicitWidth * preview.height / implicitHeight))
                    } else if (implicitHeight > 0 && page.coverFillMode === Image.PreserveAspectFit) {
                        return Math.min(
                                    preview.width,
                                    Math.round(implicitWidth * preview.height / implicitHeight))
                    } else {
                        return preview.width
                    }
                }

                height: implicitWidth > 0
                        ? Math.round(implicitHeight * width / implicitWidth)
                        : preview.height

                Component.onCompleted: {
                    data = contentsModel.thumbnail(page.currentIndex(), preview.height)
                }
            }
        }
    }

    Calligra.ContentsModel {
        id: contentsModel

        document: doc
        thumbnailSize: Theme.coverSizeLarge
    }

    Calligra.Document {
        id: doc

        readonly property bool failure: status === Calligra.DocumentStatus.Failed
        readOnly: true
        onStatusChanged: {
            if (status === Calligra.DocumentStatus.Failed) {
                errorLoader.setSource(Qt.resolvedUrl("FullscreenError.qml"), { error: lastError })
            }
        }
    }

    Loader {
        id: errorLoader
        anchors.fill: parent
    }
}
