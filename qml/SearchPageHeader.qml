/*
 * Copyright (C) 2015 Caliste Damien.
 * Contact: Damien Caliste <dcaliste@free.fr>
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

FocusScope {
    property alias title: pageHeader.title
    property alias searchText: searchField.text

    implicitHeight: col.height

    function enableSearch() {
        if (searchField.enabled) {
            searchField.forceActiveFocus()
        } else {
            searchField.enabled = true
        }
    }

    Column {
        id: col
        width: parent.width

        PageHeader {
            id: pageHeader
            width: parent.width
            visible: searchField.opacity === 0.
        }

        SearchField {
            id: searchField
            property bool open: opacity === 1.0

            width: parent.width
            enabled: false
            opacity: enabled ? 1.0 : 0.0
            visible: opacity > 0.

            //: Document search field placeholder text
            //% "Search documents"
            placeholderText: qsTrId("sailfish-office-tf-search-documents")

            // We prefer lowercase
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhPreferLowercase | Qt.ImhNoPredictiveText
            EnterKey.iconSource: "image://theme/icon-m-enter-close"
            EnterKey.onClicked: focus = false

            onOpenChanged: if (open) forceActiveFocus()
            onActiveFocusChanged: if (!activeFocus && text == "") enabled = false

            Behavior on opacity { FadeAnimation { duration: 300 } }
        }
    }
}