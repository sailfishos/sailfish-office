/*
 * Copyright (C) 2016 Caliste Damien.
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

Dialog {
    id: root

    property alias text: areaContents.text
    property bool isTextAnnotation

    SilicaFlickable {
        id: flickable

        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content

            width: parent.width
            DialogHeader {
                id: dialogHeader

                //% "Save"
                acceptText: qsTrId("sailfish-office-he-txt-anno-save")
                //% "Cancel"
                cancelText: qsTrId("sailfish-office-he-txt-anno-cancel")
            }
            TextArea {
                id: areaContents

                width: parent.width
                height: Math.max(flickable.height - dialogHeader.height, implicitHeight)
                placeholderText: isTextAnnotation
                                 ? //% "Write a note…"
                                   qsTrId("sailfish-office-ta-text-annotation")
                                 : //% "Write a comment…"
                                   qsTrId("sailfish-office-ta-comment-annotation")
                background: null
                focus: true
            }
        }
        VerticalScrollDecorator { flickable: flickable }
    }
}
