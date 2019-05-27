/*
 * Copyright (C) 2019 Jolla Ltd.
 * Contact: Pekka Vuorela <pekka.vuorela@jolla.com>
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

import QtQuick 2.6
import Sailfish.Silica 1.0

TouchBlocker {
    id: root

    property string error

    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        opacity: 0.4
        color: Theme.highlightDimmerColor
    }

    Column {
        x: Theme.horizontalPageMargin
        width: parent.width - 2*x
        spacing: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter

        HighlightImage {
            id: warningIcon

            anchors.horizontalCenter: parent.horizontalCenter
            source: "image://theme/icon-l-attention"
            highlighted: true
        }

        Label {
            width: parent.width
            text: error
            wrapMode: Text.Wrap
            color: Theme.highlightColor
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
