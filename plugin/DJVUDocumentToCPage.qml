/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Copyright (C) 2022 Yura Beznos <yura.beznos@you-ra.info>
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

Page {
    id: page

    property int pageCount
    property alias tocModel: tocListView.model

    signal pageSelected(int pageNumber)

    allowedOrientations: Orientation.All

    onTocModelChanged: tocModel.requestToc()

    SilicaListView {
        id: tocListView

        width: parent.width
        height: parent.height - gotoPage.height
        clip: true

        //: Page with DJVU index
        //% "Index"
        header: PageHeader { title: qsTrId("sailfish-office-he-djvu_index") }

        ViewPlaceholder {
            id: placeholder
            //enabled: tocListView.model
            //         && tocListView.model.ready
            //         && tocListView.model.count == 0
            //% "Document has no table of content"
            text: qsTrId("sailfish-office-me-no-toc")
        }
        PageBusyIndicator {
            //running: !tocListView.model || !tocListView.model.ready
            z: 1
        }

        delegate: BackgroundItem {
            id: bg

            Label {
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin + (Theme.paddingLarge * model.level)
                    right: pageNumberLabel.left
                    rightMargin: Theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
                elide: Text.ElideRight
                text: (model.title === undefined) ? "" : model.title
                color: bg.highlighted ? Theme.highlightColor : Theme.primaryColor
                truncationMode: TruncationMode.Fade
            }
            Label {
                id: pageNumberLabel
                anchors {
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }
                text: (model.pageNumber === undefined) ? "" : model.pageNumber
                color: bg.highlighted ? Theme.highlightColor : Theme.primaryColor
            }

            onClicked: {
                page.pageSelected(model.pageNumber - 1)
                pageStack.navigateBack(PageStackAction.Animated)
            }
        }
        
        VerticalScrollDecorator { }
    }

    PanelBackground {
        id: gotoPage

        anchors.top: tocListView.bottom
        width: parent.width
        height: Theme.itemSizeMedium

        TextField {
            property IntValidator _validator: IntValidator {bottom: 1; top: page.pageCount }

            x: Theme.paddingLarge
            width: parent.width - Theme.paddingMedium - Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter

            //% "Go to page"
            placeholderText: qsTrId("sailfish-office-lb-goto-page")
            //% "document has %n pages"
            label: qsTrId("sailfish-office-lb-%n-pages", page.pageCount)

            // We enter page numbers
            validator: text.length ? _validator : null
            inputMethodHints: Qt.ImhDigitsOnly
            EnterKey.enabled: text.length > 0 && acceptableInput
            EnterKey.iconSource: "image://theme/icon-m-enter-accept"
            EnterKey.onClicked: {
                page.pageSelected(Math.round(text) - 1)
                pageStack.navigateBack(PageStackAction.Animated)
            }
        }
    }
}
