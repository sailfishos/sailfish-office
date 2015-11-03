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
import Sailfish.Office.PDF 1.0 as PDF
import org.nemomobile.configuration 1.0
import QtQuick.LocalStorage 2.0
import "PDFStorage.js" as PDFStorage

DocumentPage {
    id: base

    property var _settings // Handle save and restore the view settings using PDFStorage

    busy: (!pdfDocument.loaded && !pdfDocument.failure) || pdfDocument.searching
    source: pdfDocument.source
    indexCount: pdfDocument.pageCount

    function savePageSettings() {
        if (!rememberPositionConfig.value) { return }
        
        if (!_settings) {
            _settings = new PDFStorage.Settings(pdfDocument.source)
        }
        var last = view.getPagePosition()
        _settings.setLastPage(last[0] + 1, last[1], last[2], view.itemWidth)
    }

    attachedPage: Component {
        PDFDocumentToCPage {
            tocModel: pdfDocument.tocModel
            pageCount: pdfDocument.pageCount
            onPageSelected: view.goToPage(pageNumber)
        }
    }

    // Save and restore view settings when needed.
    onStatusChanged: if (status == PageStatus.Inactive) { savePageSettings() }

    Connections {
        target: Qt.application
        onAboutToQuit: savePageSettings()
    }
    Connections {
        target: view
        onPageSizesReady: {
            if (rememberPositionConfig.value) {
                if (!_settings) {
                    _settings = new PDFStorage.Settings(pdfDocument.source)
                }
                var last = _settings.getLastPage()
                if (last[3] > 0) {
                    view.itemWidth = last[3]
                    view.adjust()
                }
                view.goToPage( last[0] - 1, last[1], last[2] )
            }
        }
    }

    PDFView {
        id: view

        width: base.width
        height: base.height
        document: pdfDocument

        onClicked: base.open = !base.open

        ViewPlaceholder {
            id: documentPlaceholder
            enabled: pdfDocument.failure || pdfDocument.locked
            y: (flickable ? flickable.originY : 0) + (base.height - height - (passwd.visible ? passwd.height : 0)) / 2
            //% "Broken file"
            text: pdfDocument.failure ? qsTrId("sailfish-office-me-broken-pdf")
                                      : //% "Locked file"
                                        qsTrId("sailfish-office-me-locked-pdf")
            //% "Cannot read the PDF document"
            hintText: pdfDocument.failure ? qsTrId("sailfish-office-me-broken-pdf-hint")
                                          : //% "Enter password to unlock"
                                            qsTrId("sailfish-office-me-locked-pdf-hint")
            MouseArea {
                anchors.fill: parent
                onClicked: base.open = !base.open
            }
            TextField {
                id: passwd
                visible: pdfDocument.locked
                width: parent.width - Theme.paddingLarge
                anchors.top: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                //% "password"
                label: qsTrId("sailfish-office-la-password")
                placeholderText: label

                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
                echoMode: TextInput.Password
                EnterKey.enabled: text
                EnterKey.onClicked: {
                    focus = false
                    pdfDocument.requestUnLock(text)
                    text = ""
                }

                onFocusChanged: if (focus) base.open = false
            }
        }
    }

    ToolBar {
        id: toolbar
        width: parent.width
        height: base.orientation == Orientation.Portrait || base.orientation == Orientation.InvertedPortrait
                ? Theme.itemSizeLarge
                : Theme.itemSizeSmall
        parentHeight: base.height
        flickable: view
        hidden: base.open || documentPlaceholder.enabled
        autoHide: search.text.length == 0 && !search.activeFocus

        // Toolbar contain.
        Row {
            id: row
            height: parent.height
            x: search.activeFocus ? -pageCount.width : 0

            Behavior on x {
                NumberAnimation { easing.type: Easing.InOutQuad; duration: 400 }
            }

            Item {
                anchors.verticalCenter: parent.verticalCenter
                width: pageCount.width
                height: pageCount.height
                Row {
                    id: pageCount
                    Image {
                        source: "image://theme/icon-m-document"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        text: view.currentPage + " / " + view.document.pageCount
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: base.pushAttachedPage()
                }
            }
            SearchField {
                id: search
                width: activeFocus ? toolbar.width
                                   : toolbar.width - pageCount.width - (pdfDocument.searchModel
                                                                        ? searchPrev.width + searchNext.width : 0)
                anchors.verticalCenter: parent.verticalCenter

                enabled: !pdfDocument.searching

                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhPreferLowercase | Qt.ImhNoPredictiveText
                EnterKey.onClicked: { focus = false; pdfDocument.search(text, view.currentPage - 1) }

                Behavior on width {
                    NumberAnimation { easing.type: Easing.InOutQuad; duration: 400 }
                }
            }
            IconButton {
                id: searchPrev
                anchors.verticalCenter: parent.verticalCenter
                icon.source: "image://theme/icon-m-left"
                enabled: pdfDocument.searchModel && pdfDocument.searchModel.count > 0
                onClicked: view.prevSearchMatch()
            }
            IconButton {
                id: searchNext
                anchors.verticalCenter: parent.verticalCenter
                icon.source: "image://theme/icon-m-right"
                enabled: pdfDocument.searchModel && pdfDocument.searchModel.count > 0
                onClicked: view.nextSearchMatch()
            }
        }
        // Additional information
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            
            opacity: pdfDocument.searchModel && !search.activeFocus ? 1. : 0.
            visible: opacity > 0.
            text: pdfDocument.searchModel && pdfDocument.searchModel.count > 0
                  ? //% "%n item(s) found"
                    qsTrId("sailfish-office-lb-%n-matches", pdfDocument.searchModel.count)
                  : //% "no matching found"
                    qsTrId("sailfish-office-lb-no-matches")
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.secondaryHighlightColor

            Behavior on opacity {
                FadeAnimation {}
            }
        }
    }

    PDF.Document {
        id: pdfDocument
        source: base.path
    }

    ConfigurationValue {
        id: rememberPositionConfig
        
        key: "/apps/sailfish-office/settings/rememberPosition"
        defaultValue: true
    }

    Timer {
        id: updateSourceSizeTimer
        interval: 5000
        onTriggered: linkArea.sourceSize = Qt.size(base.width, pdfCanvas.height)
    }
}
