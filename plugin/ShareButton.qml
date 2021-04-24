/*
 * Copyright (C) 2019 Jolla Ltd.
 * Copyright (C) 2021 Open Mobile Platform LLC.
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
import Sailfish.Share 1.0

IconButton {
    property DocumentPage page
    icon.source: "image://theme/icon-m-share"
    visible: page.source != ""  && !page.error
    anchors.verticalCenter: parent.verticalCenter
    onClicked: {
        shareAction.trigger()
    }

    ShareAction {
        id: shareAction

        resources: [page.source]
        mimeType: page.mimeType
    }
}
