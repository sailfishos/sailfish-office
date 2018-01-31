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
import Sailfish.TransferEngine 1.0
import Sailfish.Accounts 1.0
import com.jolla.settings.accounts 1.0
import com.jolla.signonuiservice 1.0

ShareMethodList {
    id: menuList
    objectName: "menuList"

    property Item visualParent
    property string title
    property string path
    property string mimeType

    model: SailfishTransferMethodsModel {
        id: transferMethodsModel
        filter: menuList.mimeType
    }
    source: menuList.path
    popSharingPage: false

    header: PageHeader {
        title: menuList.title
        //: Share documents
        //% "Share"
        description: qsTrId("sailfish-office-la-share")
    }

    // Add "add account" to the footer. User must be able to
    // create accounts in a case there are none.
    footer: BackgroundItem {
        Label {
            //: Add a share account
            //% "Add account"
            text: qsTrId("sailfish-office-me-add_account")
            x: Theme.horizontalPageMargin
            width: parent.width - x*2
            truncationMode: TruncationMode.Fade
            anchors.verticalCenter: parent.verticalCenter
            color: highlighted ? Theme.highlightColor : Theme.primaryColor
            visible: true
        }

        onClicked: {
            jolla_signon_ui_service.inProcessParent = visualParent
            accountCreator.startAccountCreation()
        }
    }

    SignonUiService {
        id: jolla_signon_ui_service;
        inProcessServiceName: "org.sailfish.office"
        inProcessObjectPath: "/SailfishOfficeSignonUi"
    }

    AccountCreationManager {
        id: accountCreator
        endDestination: menuList.visualParent
        endDestinationAction: PageStackAction.Pop
    }
}
