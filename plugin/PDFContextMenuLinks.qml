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

ContextMenu {
    id: contextMenuLinks

    property alias url: linkTarget.text

    InfoLabel {
        id: linkTarget
        font.pixelSize: Theme.fontSizeSmall
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        maximumLineCount: 4
        color: Theme.highlightColor
        opacity: .6
    }
    MenuItem {
        text: (contextMenuLinks.url.indexOf("http:") === 0
               || contextMenuLinks.url.indexOf("https:") === 0)
              //% "Open in browser"
              ? qsTrId("sailfish-office-me-pdf-open-browser")
              //% "Open in external application"
              : qsTrId("sailfish-office-me-pdf-open-external")
        onClicked: Qt.openUrlExternally(contextMenuLinks.url)
    }
    MenuItem {
        //% "Copy to clipboard"
        text: qsTrId("sailfish-office-me-pdf-copy-link")
        onClicked: Clipboard.text = contextMenuLinks.url
    }
}
