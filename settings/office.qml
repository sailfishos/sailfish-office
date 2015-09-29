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
import org.nemomobile.configuration 1.0


Page {
    Column {
        width: parent.width - 2 * Theme.horizontalPageMargin
        anchors.horizontalCenter: parent.horizontalCenter
        PageHeader {
            //% "Documents"
            title: qsTrId("settings_office-ph-documents")
        }
        TextSwitch {
            //% "Remember page position"
            text: qsTrId("settings_office-ts-remember")
            checked: rememberPositionConfig.value
            //% "Save page position and zoom level for a PDF document."
            description: qsTrId("settings_office-ts-remember-desc")
            onClicked: rememberPositionConfig.value = checked
            width: parent.width
        }

    }
    
    ConfigurationValue {
        id: rememberPositionConfig

        key: "/apps/sailfish-office/settings/rememberPosition"
        defaultValue: false
    }
}
