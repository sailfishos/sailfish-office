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
import Sailfish.Office 1.0
import Sailfish.TextLinking 1.0

DocumentPage {
    id: documentPage

    property real fontSize: Theme.fontSizeMedium
    property real maximumWidth
    property bool wrap: true

    busy: model.status === PlainTextModel.Loading && model.count === 0
    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if (status === PageStatus.Active) {
            model.source = documentPage.source
        }
    }

    LinkHandler {
        id: linkHandler
    }

    Flickable {
        id: horizontalFlickable

        width: documentPage.width
        height: documentPage.height - toolbar.offset

        boundsBehavior: Flickable.StopAtBounds

        contentWidth: documentPage.wrap
                    ? width
                    : Math.max(width, documentPage.maximumWidth + (2 * Theme.horizontalPageMargin))

        flickableDirection: Flickable.HorizontalFlick

        clip: toolbar.offset > 0

        SilicaListView {
            id: documentView

            width: horizontalFlickable.contentWidth
            height: documentPage.height

            _quickScrollItem.rightMargin: horizontalFlickable.contentWidth - horizontalFlickable.width - horizontalFlickable.contentX

            model: PlainTextModel {
                id: model
            }

            header: DocumentHeader {
                x: horizontalFlickable.contentX
                page: documentPage
                width: documentPage.width
            }

            delegate: Rectangle {
                width: horizontalFlickable.contentWidth
                height: line.implicitHeight
                        + (index == 0 ? Theme.paddingLarge : 0)
                        + (index == documentView.count - 1 ? Theme.paddingLarge : 0)

                Text {
                    id: line
                    x: Theme.horizontalPageMargin
                    y: index == 0 ? Theme.paddingLarge : 0
                    width: parent.width - (2 * x)
                    wrapMode: documentPage.wrap ? Text.Wrap : Text.NoWrap
                    color: Theme.darkPrimaryColor
                    linkColor: Theme.highlightColor
                    font.pixelSize: Math.round(documentPage.fontSize)
                    text: lineText
                    textFormat: Text.StyledText

                    onImplicitWidthChanged: {
                        if (implicitWidth > documentPage.maximumWidth) {
                            documentPage.maximumWidth = Math.ceil(implicitWidth)
                        }
                    }

                    onLinkActivated: linkHandler.handleLink(link)
                }
            }

            ViewPlaceholder {
                enabled: model.lineCount === 0
                         && (model.status === PlainTextModel.Ready || model.status === PlainTextModel.Error)
                text: model.status === PlainTextModel.Error
                        //% "Error loading text file"
                        ? qsTrId("sailfish-office-la-plain_text_error")
                          //% "Empty text file"
                        : qsTrId("sailfish-office-la-plain_text_empty")
            }

            VerticalScrollDecorator {
                color: Theme.highlightDimmerColor
                anchors.rightMargin: horizontalFlickable.contentWidth - horizontalFlickable.width - horizontalFlickable.contentX
            }

        }
        HorizontalScrollDecorator { color: Theme.highlightDimmerColor }
    }

    ToolBar {
        id: toolbar

        y: horizontalFlickable.height

        flickable: documentView
        enabled: !documentPage.busy
        opacity: enabled ? 1.0 : 0.0
        Behavior on opacity { FadeAnimator { duration: 400 }}

        DeleteButton {
            page: documentPage
        }

        ShareButton {
            page: documentPage
        }
    }
}
