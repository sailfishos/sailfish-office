/*
 * Copyright (C) 2019 - 2021 Open Mobile Platform LLC.
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
import Sailfish.Share 1.0

Page {
    id: page

    property alias model: filteredModel.sourceModel
    property string title
    property string searchText: searchField.text
    property bool searchEnabled
    property QtObject provider

    property string deletingSource

    function deleteSource(source) {
        pageStack.pop()
        deletingSource = source
        var popup = Remorse.popupAction(
                    page,
                    Remorse.deletedText,
                    function() {
                        provider.deleteFile(deletingSource)
                        deletingSource = ""
                    })
        popup.canceled.connect(function() { deletingSource = "" })
    }

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
    
    function getSortParameterName(parameter) {
        if (parameter === FilterModel.Name) {
            //% "name"
            return qsTrId("sailfish_office-me-sort_by_name")
        } else if (parameter === FilterModel.Type) {
            //% "type"
            return qsTrId("sailfish_office-me-sort_by_type")
        } else if (parameter === FilterModel.Date) {
            //% "date"
            return qsTrId("sailfish_office-me-sort_by_date")
        }

        return ""
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
            height: pageHeader.height + (searchEnabled ? searchField.height : 0)
            Behavior on height {
                NumberAnimation {
                    duration: 150
                    easing.type: Easing.InOutQuad
                }
            }

            PageHeader {
                id: pageHeader
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
            
            MenuItem {
                //% "Sort by: %1"
                text: qsTrId("sailfish-office-me-sort_by").arg(getSortParameterName(filteredModel.sortParameter))
                onClicked: {
                    var obj = pageStack.animatorPush("SortTypeSelectionPage.qml")
                    obj.pageCompleted.connect(function(page) {
                        page.sortSelected.connect(function(sortParameter) {
                            filteredModel.sortParameter = sortParameter
                            pageStack.pop()
                        })
                    })
                }
            }
        }

        InfoLabel {
            parent: listView.contentItem
            y: listView.headerItem.y + pageHeader.height + searchField.height
               + (page.isPortrait ? Theme.itemSizeMedium : Theme.paddingLarge)
            text: page.provider.error ? //% "Error getting document list"
                                        qsTrId("sailfish-office-la-error_getting_documents")
                                      : page.provider.count == 0
                                        ? //: View placeholder shown when there are no documents
                                          //% "No documents"
                                          qsTrId("sailfish-office-la-no_documents")
                                        : //% "No documents found"
                                          qsTrId("sailfish-office-la-not-found")
            opacity: (page.provider.ready && page.provider.count == 0)
                     || (searchText.length > 0 && listView.count == 0)
                     || page.provider.error
                     ? 1.0 : 0.0
            Behavior on opacity { FadeAnimator {} }
        }

        delegate: ListItem {
            id: listItem

            hidden: deletingSource === model.filePath
            contentHeight: Math.max(Theme.itemSizeMedium, labels.height + 2 * Theme.paddingMedium)

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
                id: labels
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
                    pageStack.animatorPush("Sailfish.Office.TextDocumentPage",
                                           { title: model.fileName, source: model.filePath, mimeType: model.fileMimeType, provider: page.provider })
                    break
                case DocumentListModel.PlainTextDocument:
                    pageStack.animatorPush("Sailfish.Office.PlainTextDocumentPage",
                                           { title: model.fileName, source: model.filePath, mimeType: model.fileMimeType, provider: page.provider })
                    break
                case DocumentListModel.SpreadSheetDocument:
                    pageStack.animatorPush("Sailfish.Office.SpreadsheetPage",
                                           { title: model.fileName, source: model.filePath, mimeType: model.fileMimeType, provider: page.provider })
                    break
                case DocumentListModel.PresentationDocument:
                    pageStack.animatorPush("Sailfish.Office.PresentationPage",
                                           { title: model.fileName, source: model.filePath, mimeType: model.fileMimeType, provider: page.provider })
                    break
                case DocumentListModel.PDFDocument:
                    pageStack.animatorPush("Sailfish.Office.PDFDocumentPage",
                                           { title: model.fileName, source: model.filePath, mimeType: model.fileMimeType, provider: page.provider })
                    break
                default:
                    console.log("Unknown file format for file " + model.fileName + " with stated mimetype " + model.fileMimeType)
                    break
                }
            }

            function deleteFile() {
                remorseDelete(function() { page.provider.deleteFile(model.filePath) })
            }

            // TODO: transitions disabled until they don't anymore confuse SilicaListView positioning. JB#33215
            //ListView.onAdd: AddAnimation { target: listItem }
            //ListView.onRemove: RemoveAnimation { target: listItem }

            menu: Component {
                ContextMenu {
                    id: contextMenu
                    MenuItem {
                        //: Share a file
                        //% "Share"
                        text: qsTrId("sailfish-office-la-share")
                        onClicked: {
                            shareAction.resources = [model.filePath]
                            shareAction.trigger()
                        }
                        ShareAction {
                            id: shareAction
                            mimeType: model.fileMimeType
                        }
                    }
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
}
