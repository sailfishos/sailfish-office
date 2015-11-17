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

Item {
    id: root

    property alias text: label.text
    property bool multiLine
    property string iconSource

    Image {
        id: icon

        anchors {
            left: parent.left
            leftMargin: Theme.paddingLarge
            verticalCenter: root.multiLine ? undefined : parent.verticalCenter
        }
        source: root.iconSource !== "" ? root.iconSource
                                       : "image://theme/icon-l-document"

        sourceSize {
            width: Theme.iconSizeSmall
            height: Theme.iconSizeSmall
        }
    }
    Label {
        id: label

        anchors {
            left: icon.right
            leftMargin: Theme.paddingMedium
            verticalCenter: root.multiLine ? undefined : parent.verticalCenter
            right: parent.right
            rightMargin: Theme.paddingLarge
        }

        truncationMode: root.multiLine ? TruncationMode.None : TruncationMode.Fade
        wrapMode: root.multiLine ? Text.WrapAnywhere : Text.NoWrap
    }
}
