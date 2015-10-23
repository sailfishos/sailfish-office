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
    property string searchText
    property QtObject provider

    allowedOrientations: Orientation.All

    FilterModel {
        id: filteredModel
        filterRegExp: RegExp(searchText, "i")
    }

    SilicaListView {
        id: listView

        anchors.fill: parent
        model: filteredModel
        currentIndex: -1 // otherwise currentItem will steal focus

        header: SearchPageHeader {
            id: header
            width: parent.width

            //: Application title
            //% "Documents"
            title: qsTrId("sailfish-office-he-apptitle")

            // TODO: uncomment once there are more document sources
            // title: page.title

            Binding {
                target: page
                property: "searchText"
                value: header.searchText
            }

            Connections {
                target: menuItemSearch
                onClicked: header.enableSearch()
            }
        }

        PullDownMenu {
            visible: listView.count > 0
            MenuItem {
                id: menuItemSearch

                //: Search menu entry
                //% "Search"
                text: qsTrId("sailfish-office-me-search")
            }
        }
        
        ViewPlaceholder {
            //: View placeholder shown when there are no documents
            //% "No documents"
            text: searchText.length == 0 ? qsTrId("sailfish-office-la-no_documents")
                                         : //% "No document found"
                                           qsTrId("sailfish-office-la-not-found")
            enabled: !listView.count
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
                source: fileMimeType
                states: State {
                    when: icon.fileMimeType === ""
                    PropertyChanges {
                        target: icon
                        source: "image://theme/icon-l-document?"
                                + (listItem.highlighted ? Theme.highlightColor : Theme.primaryColor)
                    }
                }
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

            ListView.onAdd: AddAnimation { target: listItem }
            ListView.onRemove: RemoveAnimation { target: listItem }

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
