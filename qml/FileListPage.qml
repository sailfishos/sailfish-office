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
import Sailfish.Office.Files 1.0

Page {
    id: page

    property alias model: filteredModel.sourceModel
    property string title
    property string searchText: searchField.text
    property bool searchEnabled
    property QtObject provider

    allowedOrientations: Orientation.All

    onSearchEnabledChanged: {
        if (pageStack.currentPage.status == PageStatus.Active) {
            if (searchEnabled) {
                searchField.forceActiveFocus()
            } else {
                searchField.focus = false
            }
        }
        if (!searchEnabled) {
            searchField.text = ""
        }
    }

    FilterModel {
        id: filteredModel
        filterRegExp: RegExp(searchText, "i")
    }

    SilicaListView {
        id: listView

        anchors.fill: parent
        model: filteredModel
        currentIndex: -1 // otherwise currentItem will steal focus

        header: Item {
            width: listView.width
            height: headerContent.height
        }

        Column {
            id: headerContent

            parent: listView.headerItem
            width: parent.width

            PageHeader {
                //: Application title
                //% "Documents"
                title: qsTrId("sailfish-office-he-apptitle")
            }

            SearchField {
                id: searchField

                width: parent.width
                opacity: page.searchEnabled ? 1.0 : 0.0
                visible: opacity > 0

                //: Document search field placeholder text
                //% "Search documents"
                placeholderText: qsTrId("sailfish-office-tf-search-documents")

                // We prefer lowercase
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhPreferLowercase | Qt.ImhNoPredictiveText
                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: focus = false

                Behavior on opacity { FadeAnimation { duration: 150 } }
                Behavior on height {
                    NumberAnimation {
                        duration: 150
                        easing.type: Easing.InOutQuad
                    }
                }
                Binding {
                    target: searchField
                    when: !searchEnabled
                    property: "height"
                    value: 0
                }
            }
        }

        Connections {
            target: searchField.activeFocus ? listView : null
            ignoreUnknownSignals: true
            onContentYChanged: {
                if (listView.contentY > (Screen.height / 2)) {
                    searchField.focus = false
                }
            }
        }

        PullDownMenu {
            id: menu

            property bool _searchEnabled

            // avoid changing text state while menu is open
            onActiveChanged: {
                if (active) {
                    _searchEnabled = page.searchEnabled
                }
            }

            MenuItem {
                text: !menu._searchEnabled ? //% "Show search"
                                             qsTrId("sailfish-office-me-show_search")
                                             //% "Hide search"
                                           : qsTrId("sailfish-office-me-hide_search")
                onClicked: page.searchEnabled = !page.searchEnabled
            }
        }

        InfoLabel {
            parent: listView.contentItem
            y: listView.headerItem.y + listView.headerItem.height + Theme.paddingLarge
            //: View placeholder shown when there are no documents
            //% "No documents"
            text: searchText.length == 0 ? qsTrId("sailfish-office-la-no_documents")
                                         : //% "No documents found"
                                           qsTrId("sailfish-office-la-not-found")
            visible: opacity > 0
            opacity: listView.count > 0 ? 0.0 : 1.0
            Behavior on opacity { FadeAnimation {} }
        }

        delegate: ListItem {
            id: listItem
            contentHeight: Theme.itemSizeMedium

            Image {
                id: icon
                property string fileMimeType: window.mimeToIcon(model.fileMimeType)
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }
                source: fileMimeType !== "" ? fileMimeType
                                            : ("image://theme/icon-m-document?"
                                               + (listItem.highlighted ? Theme.highlightColor : Theme.primaryColor))
            }
            Label {
                anchors {
                    left: icon.right
                    leftMargin: Theme.paddingMedium
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    bottom: icon.verticalCenter
                }
                color: listItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                text: searchText.length > 0 ? Theme.highlightText(model.fileName, searchText, Theme.highlightColor)
                                            : model.fileName
                textFormat: searchText.length > 0 ? Text.StyledText : Text.PlainText
                font.pixelSize: Theme.fontSizeMedium
                truncationMode: TruncationMode.Fade
            }
            Label {
                anchors {
                    left: icon.right
                    leftMargin: Theme.paddingMedium
                    top: icon.verticalCenter
                }
                text: Format.formatFileSize(model.fileSize)

                font.pixelSize: Theme.fontSizeExtraSmall
                color: listItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
            }
            Label {
                anchors {
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    top: icon.verticalCenter
                }

                text: Format.formatDate(model.fileRead, Format.Timepoint)

                font.pixelSize: Theme.fontSizeExtraSmall
                color: listItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
            }

            onClicked: {
                switch(model.fileDocumentClass) {
                    case DocumentListModel.TextDocument:
                        pageStack.push(pages.textDocument, { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType });
                        break;
                    case DocumentListModel.SpreadSheetDocument:
                        pageStack.push(pages.spreadsheet, { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType });
                        break;
                    case DocumentListModel.PresentationDocument:
                        pageStack.push(pages.presentation, { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType });
                        break;
                    case DocumentListModel.PDFDocument:
                        pageStack.push(pages.pdf, { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType });
                        break;
                    default:
                        console.log("Unknown file format for file " + model.fileName + " with stated mimetype " + model.fileMimeType);
                        break;
                }
            }

            function deleteFile() {
                var idx = index
                //: Deleting file after timeout.
                //% "Deleting"
                remorseAction(qsTrId("sailfish-office-la-deleting"), function() { page.provider.deleteFile(model.filePath) })
            }

            // TODO: transitions disabled until they don't anymore confuse SilicaListView positioning. JB#33215
            //ListView.onAdd: AddAnimation { target: listItem }
            //ListView.onRemove: RemoveAnimation { target: listItem }

            menu: Component {
                ContextMenu {
                    id: contextMenu
                    MenuItem {
                        //: Delete a file from the device
                        //% "Delete"
                        text: qsTrId("sailfish-office-me-delete")
                        onClicked: {
                            listItem.deleteFile()
                        }
                    }
                }
            }
        }

        VerticalScrollDecorator { }
    }

    DocumentPages {
        id: pages
    }
}
