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

            Flow {
                width: parent.width - 2 * Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter
                opacity: filteredModel.tagFiltered ? 1.0 : 0.0
                visible: opacity > 0

                spacing: Theme.paddingMedium

                Label {
                    //% "Filtered by: "
                    text: qsTrId("sailfish-office-lbl-tag-filter")
                    color: Theme.secondaryHighlightColor
                    font.pixelSize: Theme.fontSizeSmall
                }

                Repeater {
                    id: tags
                    delegate: Tag {
                        id: tagDelegate

                        enabled: false
                        tag: model.label
                        fontSize: Theme.fontSizeSmall
                        visible: selected
                        Connections {
                            target: filteredModel
                            onTagFilteringChanged: selected = filteredModel.hasTag(model.label)
                        }
                    }
                    model: page.model.tags
                }

                Behavior on opacity { FadeAnimation { duration: 150 } }
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
                //: Tag filter menu entry
                //% "Filter by tags"
                text: qsTrId("sailfish-office-me-tag-filter")
                enabled: page.model.tags.count > 0
                onClicked: pageStack.push("TagsSelector.qml", {
                    //% "Filtering tags"
                    title: qsTrId("sailfish-office-he-filtering-tags"),
                    model: page.model.tags,
                    highlight: filteredModel })
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
            text: (searchText.length == 0 && !filteredModel.tagFiltered) ?
                  //% "No documents"
                  qsTrId("sailfish-office-la-no_documents")
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
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }
                source: window.mimeToIcon(model.fileMimeType) + (highlighted ? "?" + Theme.highlightColor : "")
            }
            Column {
                anchors {
                    left: icon.right
                    leftMargin: Theme.paddingMedium
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }
                Label {
                    id: label
                    width: parent.width
                    color: listItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                    text: searchText.length > 0 ? Theme.highlightText(model.fileName, searchText, Theme.highlightColor)
                                                : model.fileName
                    textFormat: searchText.length > 0 ? Text.StyledText : Text.PlainText
                    font.pixelSize: Theme.fontSizeMedium
                    truncationMode: TruncationMode.Fade
                }
                Item {
                    width: parent.width
                    height: sizeLabel.height
                    Label {
                        id: sizeLabel
                        text: Format.formatFileSize(model.fileSize)
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: listItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    }
                    Label {
                        anchors.right: parent.right
                        text: Format.formatDate(model.fileRead, Format.Timepoint)
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: listItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    }
                }
            }

            onClicked: {
                switch(model.fileDocumentClass) {
                case DocumentListModel.TextDocument:
                    pageStack.push("Sailfish.Office.TextDocumentPage",
                                   { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType })
                    break
                case DocumentListModel.SpreadSheetDocument:
                    pageStack.push("Sailfish.Office.SpreadsheetPage",
                                   { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType })
                    break
                case DocumentListModel.PresentationDocument:
                    pageStack.push("Sailfish.Office.PresentationPage",
                                   { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType })
                    break
                case DocumentListModel.PDFDocument:
                    pageStack.push("Sailfish.Office.PDFDocumentPage",
                                   { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType })
                    break
                default:
                    console.log("Unknown file format for file " + model.fileName + " with stated mimetype " + model.fileMimeType)
                    break
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
                    MenuItem {
                        //% "Manage tags"
                        text: qsTrId("sailfish-office-me-tags")
                        onClicked: {
                            var Doc = function(model, path) {
                                this.model = model
                                this.path = path
                                this.hasTag = function(tag) { return this.model.hasTag(this.path, tag) }
                                this.addTag = function(tag) { this.model.addTag(this.path, tag) }
                                this.removeTag = function(tag) { this.model.removeTag(this.path, tag) }
                            }
                            pageStack.push("TagsSelector.qml", {
                                //% "Manage tags"
                                title: qsTrId("sailfish-office-he-manage-tags"),
                                editing: true,
                                model: page.model.tags,
                                highlight: new Doc(page.model, model.filePath) })
                        }
                    }
                }
            }
        }

        VerticalScrollDecorator { }
    }
}
