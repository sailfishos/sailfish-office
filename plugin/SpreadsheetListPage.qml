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
import org.kde.calligra 1.0 as Calligra

Page {
    id: page

    property QtObject document

    allowedOrientations: Orientation.All

    SilicaListView {
        anchors.fill: parent

        //: Page with sheet selector
        //% "Sheets"
        header: PageHeader { title: qsTrId("sailfish-office-he-sheet_index") }

        model: Calligra.ContentsModel {
            document: page.document
            thumbnailSize.width: Theme.itemSizeLarge
            thumbnailSize.height: Theme.itemSizeLarge
        }

        delegate: BackgroundItem {
            Calligra.ImageDataItem {
                id: thumbnail

                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }

                width: parent.height
                height: parent.height

                data: model.thumbnail
            }

            Label {
                anchors {
                    left: thumbnail.right
                    leftMargin: Theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }

                text: model.title
                color: (model.contentIndex == page.document.currentIndex || highlighted) ? Theme.highlightColor
                                                                                         : Theme.primaryColor
                truncationMode: TruncationMode.Fade
            }

            onClicked: {
                page.document.currentIndex = model.contentIndex
                pageStack.navigateBack()
            }
        }
    }
}
