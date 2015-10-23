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
import Sailfish.Office.Files 1.0

Page {

    allowedOrientations: Orientation.All

    Component {
        id: delegate

        BackgroundItem {
            id: delegateItem
            width: view.width
            height: thumbnail.height
            enabled: model.count > 0 || model.count === -1
            opacity: enabled ? 1.0 : 0.5

            Label {
                id: countLabel
                objectName: "countLabel"
                anchors {
                    right: thumbnail.left
                    rightMargin: Theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
                text: model.count > -1 ? model.count : ""
                color: delegateItem.down ? Theme.highlightColor : Theme.primaryColor
                font.pixelSize: Theme.fontSizeLarge
            }

            // Load icon from a plugin
            HighlightImage {
                id: thumbnail
                x: width - Theme.paddingLarge
                width: Theme.itemSizeExtraLarge
                height: width
                source: model.icon
                opacity: delegateItem.down ? 0.5 : 1
                highlighted: delegateItem.highlighted
            }

            Label {
                id: titleLabel
                objectName: "titleLabel"
                elide: Text.ElideRight
                font.pixelSize: Theme.fontSizeLarge
                text: model.title
                color: delegateItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                anchors {
                    left: thumbnail.right
                    right: parent.right
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }
            }

            onClicked: {
                if (model.needsSetup) {
                    pageStack.push(Qt.resolvedUrl(model.setupPageUrl))
                } else {
                    //console.debug("Model name: " + providerModel.objectName)
                    window.pageStack.push(model.page != "" ? Qt.resolvedUrl(model.page) : fileListPage, {
                        title: model.title,
                        model: model.providerModel,
                        provider: view.model.sources[index]
                        //thumbnailDelegate: thumbnail != "" ? thumbnail : Qt.resolvedUrl("GridImageThumbnail.qml")
                        } )
                }
            }
        }
    }

    SilicaListView {
        id: view
        objectName: "docListView"

        anchors.fill: parent
        delegate: delegate
        header: PageHeader {
            //: Application title
            //% "Documents"
            title: qsTrId("sailfish-office-he-apptitle")
        }
        model: DocumentProviderListModel {
            id: documentSources
            TrackerDocumentProvider {
            }
        }
    }

    Component {
        id: fileListPage
        FileListPage { }
    }
}
