/*
 * Copyright (C) 2024 Damien Caliste
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
import Sailfish.Office 1.0

DetailsPage {
    readonly property bool storePassword: document.password.length > 0

    DetailItem {
        //: Page count of the PDF document
        //% "Page Count"
        label: qsTrId("sailfish-office-la-pdf-pagecount")
        value: document.pageCount
        alignment: Qt.AlignLeft
        visible: !document.locked
    }
    SectionHeader {
        visible: document.passwordProtected
        //% "Read protection"
        text: qsTrId("sailfish-office-la-pdf-readprotection")
    }
    Label {
        visible: document.passwordProtected
        width: parent.width - 2*x
        x: Theme.horizontalPageMargin
        //% "This document is protected by a password"
        text: qsTrId("sailfish-office-la-pdf-passwordprotected")
        color: palette.secondaryHighlightColor
        font.pixelSize: Theme.fontSizeSmall
        textFormat: Text.PlainText
        wrapMode: Text.Wrap
    }
    Item {
        visible: storePassword
        width: parent.width
        height: Theme.paddingLarge
    }
    PasswordField {
        opacity: storePassword ? 1 : 0
        Behavior on opacity {FadeAnimator {}}
        x: Theme.horizontalPageMargin
        width: parent.width - 2*x
        text: document.password
        readOnly: true
    }
    Item {
        visible: storePassword
        width: parent.width
        height: Theme.paddingLarge
    }
    Button {
        opacity: storePassword ? 1 : 0
        Behavior on opacity {FadeAnimator {}}
        anchors.horizontalCenter: parent.horizontalCenter
        //% "Clear stored password"
        text: qsTrId("sailfish-office-la-pdf-clearpassword")
        onClicked: document.clearCachedPassword()
    }
}
