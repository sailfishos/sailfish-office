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

    property bool iconized: true
    property int modelCount: -1
    property real iconizedWidth

    property real _margin: Math.max((iconizedWidth - searchIcon.width) / 2., 0.)

    signal requestSearch(string text)
    signal requestPreviousMatch()
    signal requestNextMatch()
    signal requestCancel()

    onClicked: iconized = false
    onIconizedChanged: if (!iconized) searchField.forceActiveFocus()

    states: [State {
                 name: "extended"
                 when: !root.iconized
                 PropertyChanges {
                     target: root
                     _margin: Theme.horizontalPageMargin
                 }
             },
             State {
                 name: "iconized"
                 when: root.iconized
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

    IconButton {
        id: searchIcon
        anchors {
            left: parent.left
            leftMargin: _margin
        }
        width: icon.width
        height: parent.height
        icon.source: "image://theme/icon-m-search"
        highlighted: down || root.down || searchField.activeFocus

        onClicked: {
            root.clicked(mouse)
            searchField.forceActiveFocus()
        }
    }

    TextField {
        id: searchField

        property string _searchText

        height: Math.max(Theme.itemSizeMedium, _editor.height + Theme.paddingMedium + Theme.paddingSmall)

        anchors {
            verticalCenter: parent.verticalCenter
            left: searchIcon.right
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
        }

        focusOutBehavior: FocusBehavior.ClearPageFocus
        font {
            // visible label doesn't leave much room. match count might go away if heavy full-document search is replaced
            // with more incremental approach, so should be good for now
            pixelSize: labelVisible ? Theme.fontSizeMediumBase : Theme.fontSizeLarge
            family: Theme.fontFamilyHeading
        }

        textRightMargin: clearButton.width
            + (searchPrev.visible ? searchPrev.width + Theme.paddingLarge : 0.)
            + (searchNext.visible ? searchNext.width + Theme.paddingLarge : 0.)
        textTopMargin: labelVisible ? Theme.paddingSmall : (height/2 - _editor.implicitHeight/2)

        labelVisible: root.modelCount > 0 && !searchField.activeFocus
        //% "%n item(s) found"
        label: qsTrId("sailfish-office-lb-%n-matches", root.modelCount)

        placeholderText: (root.modelCount == 0 && !activeFocus)
            //% "No result"
            ? qsTrId("sailfish-office-search-no-result")
            //% "Search on document"
            : qsTrId("sailfish-office-search-document")

        Connections {
            target: root
            onModelCountChanged: if (modelCount == 0) {
                searchField.text = "" // Allow the placeholder
                searchField._searchText = ""
            }
        }

        onActiveFocusChanged: {
            if (activeFocus) {
                cursorPosition = text.length
            } else {
                if (text != _searchText) {
                    text = _searchText
                }
                if (!text) {
                    root.iconized = true
                }
            }
        }

        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhPreferLowercase | Qt.ImhNoPredictiveText
        EnterKey.iconSource: text != "" ? "image://theme/icon-m-enter-accept"
                                        : "image://theme/icon-m-enter-close"
        EnterKey.onClicked: {
            if (text != "") {
                _searchText = text
                root.requestSearch(text)
            } else {
                root.iconized = true
            }
            focus = false
        }

        background: null

        visible: !root.iconized && opacity > 0.

        opacity: root._margin == Theme.horizontalPageMargin ? 1. : 0.
        Behavior on opacity { FadeAnimation {} }

        Item {
            parent: searchField
            anchors.right: parent.right

            height: parent.height

            IconButton {
                id: searchPrev
                anchors {
                    right: searchNext.left
                    rightMargin: Theme.paddingLarge
                }
                width: icon.width
                height: parent.height
                icon.source: "image://theme/icon-m-left"

                visible: opacity > 0.

                opacity: root.modelCount > 0 && !searchField.activeFocus ? 1 : 0
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

                visible: opacity > 0.

                opacity: root.modelCount > 0 && !searchField.activeFocus ? 1 : 0
                Behavior on opacity { FadeAnimation {} }

                onClicked: root.requestNextMatch()
            }

            IconButton {
                id: clearButton
                anchors.right: parent.right

                width: icon.width
                height: parent.height
                icon.source: "image://theme/icon-m-clear"

                onClicked: {
                    // Cancel any pending search.
                    root.requestCancel()
                    searchField._searchText = ""
                    if (!searchField.activeFocus || searchField.text == "") {
                        // Close case.
                        root.iconized = true
                        searchField.focus = false
                    } else {
                        // Clear case.
                        searchField.text = ""
                        searchField.forceActiveFocus()
                    }
                }
            }
        }
    }
}
