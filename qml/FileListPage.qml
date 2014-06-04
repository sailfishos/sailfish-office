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
    property alias model: listView.model;
    property string title: "";
    property QtObject provider;

    allowedOrientations: Orientation.All;

    SilicaListView {
        id: listView;
        anchors.fill: parent

        currentIndex: -1;

        children: ScrollDecorator { }
        header: PageHeader {
            //: Application title
            //% "Documents"
            title: qsTrId("sailfish-office-he-apptitle");

            // TODO: uncomment once there are more document sources
            // title: page.title;
        }
        
        ViewPlaceholder {
            //: View placeholder shown when there are no documents
            //% "No documents"
            text: qsTrId("sailfish-office-la-no_documents")
            enabled: !listView.count
        }
        delegate: Item {
            id: listItem;

            property bool menuOpen: ListView.isCurrentItem;

            width: ListView.view.width;
            height: menuOpen ? contextMenu.height + bg.height : bg.height;

            BackgroundItem {
                id: bg;

                width: parent.width;

                Image {
                    id: icon;


                    property string fileMimeType: window.mimeToIcon(model.fileMimeType)
                    anchors {
                        left: parent.left;
                        leftMargin: Theme.paddingLarge;
                        verticalCenter: parent.verticalCenter;
                    }
                    source: fileMimeType
                    states: State {
                        when: icon.fileMimeType === ""
                        PropertyChanges {
                            target: icon
                            source: "image://theme/icon-l-document?"
                                    + (bg.highlighted ? Theme.highlightColor : Theme.primaryColor);
                        }
                    }
                }
                Label {
                    anchors {
                        left: icon.right;
                        leftMargin: Theme.paddingMedium;
                        right: parent.right;
                        bottom: icon.verticalCenter;
                    }
                    color: (bg.highlighted || listItem.menuOpen) ? Theme.highlightColor : Theme.primaryColor
                    text: model.fileName;
                    font.pixelSize: Theme.fontSizeMedium;
                    truncationMode: TruncationMode.Fade;
                }
                Label {
                    anchors {
                        left: icon.right;
                        leftMargin: Theme.paddingMedium;
                        top: icon.verticalCenter;
                    }
                    text: Format.formatFileSize(model.fileSize);

                    font.pixelSize: Theme.fontSizeExtraSmall;
                    color: Theme.secondaryColor;
                }
                Label {
                    anchors {
                        right: parent.right;
                        rightMargin: Theme.paddingLarge;
                        top: icon.verticalCenter;
                    }

                    text: Format.formatDate(model.fileRead, Format.Timepoint);

                    font.pixelSize: Theme.fontSizeExtraSmall;
                    color: Theme.secondaryColor;
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

                onPressAndHold: {
                    listItem.ListView.view.currentIndex = index;
                    contextMenu.show(listItem);
                }

                RemorseItem { id: remorse; }
            }

            function deleteFile() {
                var idx = index;
                //: Deleting file after timeout.
                //% "Deleting"
                remorse.execute(listItem, qsTrId("sailfish-office-la-deleting"), function() { page.provider.deleteFile(model.filePath) });
            }

            ListView.onAdd: AddAnimation { target: listItem; }
            ListView.onRemove: RemoveAnimation { target: listItem; }
        }

        ContextMenu {
            id: contextMenu;
            MenuItem {
                //: Delete a file from the device
                //% "Delete"
                text: qsTrId("sailfish-office-me-delete");
                onClicked: {
                    listView.currentItem.deleteFile();
                }
            }

            onClosed: listView.currentIndex = -1;
        }
    }

    DocumentPages {
        id: pages;
    }
}
