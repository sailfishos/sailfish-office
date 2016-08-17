/****************************************************************************************
**
** Copyright (C) 2013-2016 Jolla Ltd., Damien Caliste
** Contact: Raine Makelainen <raine.makelainen@jollamobile.com>
** Contact: Damien Caliste <dcaliste@free.fr>
** All rights reserved.
**
** This file is part of Sailfish Office package and is a modified
** version of SearchField.qml from Sailfish Silica package to add
** next and previous button, modify the clear button action to support
** cancellation and also introduce a new iconized state.
**
** You may use this file under the terms of BSD license as follows:
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Jolla Ltd nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
** ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0

BackgroundItem {
    id: root

    property alias iconized: searchField.iconized
    property alias modelCount: searchField.modelCount
    property real iconizedWidth

    signal requestSearch(string text)
    signal requestPreviousMatch()
    signal requestNextMatch()
    signal requestCancel()

    onClicked: searchField.iconized = false

    states: [State {
                 name: "extended"
                 when: !searchField.iconized
                 PropertyChanges {
                     target: searchField
                     _margin: 0.
                 }
             },
             State {
                 name: "iconized"
                 when: searchField.iconized
                 PropertyChanges {
                     target: root
                     width: iconizedWidth
                 }
             }]
    transitions: Transition {
        NumberAnimation {
            properties: "_margin, width"
            easing.type: Easing.InOutQuad
            duration: 400
        }
    }
    highlighted: down || searchIcon.down

    TextField {
        id: searchField

        property bool iconized: true
        property int modelCount: -1

        property real _prevNextWidth: modelCount > 0 ? (Theme.itemSizeSmall + Theme.paddingMedium)*2 : 0.
        implicitWidth: !iconized ? _editor.implicitWidth + Theme.paddingSmall
                                   + Theme.itemSizeSmall*2  // width of two icons
                                   + _prevNextWidth // width of prev / next icons
                                 : Theme.itemSizeSmall
        height: Math.max(Theme.itemSizeMedium, _editor.height + Theme.paddingMedium + Theme.paddingSmall)

        property real _margin: Math.max((root.iconizedWidth - Theme.itemSizeSmall) / 2., 0.)
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
            leftMargin: _margin
            rightMargin: _margin
        }

        focusOutBehavior: FocusBehavior.ClearPageFocus
        font {
            pixelSize: Theme.fontSizeLarge
            family: Theme.fontFamilyHeading
        }

        textLeftMargin: Theme.itemSizeSmall + Theme.paddingMedium + Theme.horizontalPageMargin - Theme.paddingLarge
        textRightMargin: Theme.itemSizeSmall + Theme.paddingMedium + Theme.horizontalPageMargin - Theme.paddingLarge + _prevNextWidth
        textTopMargin: labelVisible ? Theme.paddingSmall : (height/2 - _editor.implicitHeight/2)
        labelVisible: modelCount > 0 && !searchField._editor.activeFocus
        Binding on label {
            target: searchField
            //% "%n item(s) found"
            value: qsTrId("sailfish-office-lb-%n-matches", modelCount)
            when: modelCount > 0
        }

        onModelCountChanged: if (modelCount == 0) text = "" // Allow the placeholder
        // to be visible for no results.
        onIconizedChanged: if (!iconized) _editor.forceActiveFocus()

        //: Placeholder text of SearchField
        placeholderText: (modelCount == 0)
            //% "No result"
            ? qsTrId("sailfish-office-search-no-result")
            //% "Search on document"
            : qsTrId("sailfish-office-search-document")
        onFocusChanged: {
            if (focus) {
                cursorPosition = text.length
                searchField.iconized = false
            }
        }

        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhPreferLowercase | Qt.ImhNoPredictiveText
        EnterKey.iconSource: text != "" ? "image://theme/icon-m-enter-accept"
                                        : "image://theme/icon-m-enter-close"
        EnterKey.onClicked: {
            focus = false
            if (text != "") {
                root.requestSearch(text)
            } else {
                iconized = true
            }
        }

        background: null

        Item {
            parent: searchField // avoid TextBase contentItem auto-parenting
            anchors.fill: parent

            IconButton {
                id: searchIcon
                x: searchField.textLeftMargin - width - Theme.paddingSmall
                width: icon.width
                height: parent.height
                icon.source: "image://theme/icon-m-search"
                highlighted: down || root.down || searchField._editor.activeFocus

                enabled: searchField.enabled

                onClicked: {
                    searchField.iconized = false
                    searchField._editor.forceActiveFocus()
                }
            }

            IconButton {
                id: searchPrev
                anchors {
                    right: searchNext.left
                    rightMargin: Theme.paddingLarge
                }
                width: icon.width
                height: parent.height
                icon.source: "image://theme/icon-m-left"

                enabled: searchField.enabled
                visible: !iconized && opacity > 0.

                opacity: modelCount > 0 && !searchField._editor.activeFocus ? 1 : 0
                Behavior on opacity { FadeAnimation {} }

                onClicked: root.requestPreviousMatch()
            }

            IconButton {
                id: searchNext
                anchors {
                    right: clearButton.left
                    rightMargin: Theme.paddingLarge
                }
                width: icon.width
                height: parent.height
                icon.source: "image://theme/icon-m-right"

                enabled: searchField.enabled
                visible: !iconized && opacity > 0.

                opacity: modelCount > 0 && !searchField._editor.activeFocus ? 1 : 0
                Behavior on opacity { FadeAnimation {} }

                onClicked: root.requestNextMatch()
            }

            IconButton {
                id: clearButton
                anchors {
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                width: icon.width
                height: parent.height
                icon.source: "image://theme/icon-m-clear"

                enabled: searchField.enabled
                visible: !iconized && opacity > 0.

                opacity: searchField.width > 2 * Theme.itemSizeSmall ? 1. : 0.
                Behavior on opacity { FadeAnimation {} }

                onClicked: {
                    // Cancel any pending search.
                    root.requestCancel()
                    if (!searchField._editor.activeFocus || searchField.text == "") {
                        // Close case.
                        searchField.iconized = true
                        searchField.focus = false
                    } else {
                        // Clear case.
                        searchField.text = ""
                        searchField._editor.forceActiveFocus()
                    }
                }
            }
        }
    }
}