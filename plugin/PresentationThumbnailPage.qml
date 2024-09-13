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

    SilicaGridView {
        id: grid

        anchors.fill: parent

        cellWidth: page.width / 3
        cellHeight: cellWidth * 0.75

        currentIndex: page.document.currentIndex

        //: Page with slide overview
        //% "Slides"
        header: PageHeader { title: qsTrId("sailfish-office-he-slide_index") }

        model: Calligra.ContentsModel {
            document: page.document
            thumbnailSize.width: grid.cellWidth
            thumbnailSize.height: grid.cellHeight
        }

        delegate: Item {
            id: root
            width: GridView.view.cellWidth
            height: GridView.view.cellHeight

            Rectangle {
                anchors.fill: parent
                border.width: 1

                Calligra.ImageDataItem {
                    anchors.fill: parent
                    data: model.thumbnail
                }

                Rectangle {
                    anchors.centerIn: parent
                    width: label.width + Theme.paddingMedium
                    height: label.height
                    radius: Theme.paddingSmall
                    color: root.GridView.isCurrentItem ? Theme.highlightColor : Theme.darkPrimaryColor
                }

                Label {
                    id: label

                    anchors.centerIn: parent
                    text: model.contentIndex + 1
                    color: Theme.lightPrimaryColor
                }

                Rectangle {
                    anchors.fill: parent
                    color: mouseArea.pressed && mouseArea.containsMouse ? Theme.highlightBackgroundColor
                                                                        : "transparent"
                    opacity: Theme.highlightBackgroundOpacity
                }

            }

            MouseArea {
                id: mouseArea

                anchors.fill: parent
                onClicked: {
                    page.document.currentIndex = model.contentIndex
                    pageStack.navigateBack(PageStackAction.Animated)
                }
            }
        }
    }
}
