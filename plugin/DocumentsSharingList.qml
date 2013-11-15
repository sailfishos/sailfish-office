import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.TransferEngine 1.0
import Sailfish.Accounts 1.0
import com.jolla.settings.accounts 1.0
import com.jolla.signonuiservice 1.0

ShareMethodList {
    id: menuList
    objectName: "menuList"

    property Item visualParent;
    property string title;
    property string path;
    property string mimeType;

    model: SailfishTransferMethodsModel {
        id: transferMethodsModel;
        filter: menuList.mimeType;
    }
    source: menuList.path;

    //: Share documents
    //% "Share"
    listHeader: qsTrId( "sailfish-office-la-share" );

    header: PageHeader{ title: menuList.title }

    // Add "add account" to the footer. User must be able to
    // create accounts in a case there are none.
    footer: BackgroundItem {
        Label {
            //: Add a share account
            //% "Add account"
            text: qsTrId( "sailfish-office-me-add_account" );
            x: Theme.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            color: highlighted ? Theme.highlightColor : Theme.primaryColor
            visible: true
        }

        onClicked: {
            jolla_signon_ui_service.inProcessParent = visualParent
            accountCreator.startAccountCreation();
        }
    }

    SignonUiService {
        id: jolla_signon_ui_service;
        inProcessServiceName: "org.sailfish.office";
        inProcessObjectPath: "/SailfishOfficeSignonUi"
    }

    AccountCreationManager {
        id: accountCreator;
        endDestination: menuList.visualParent;
        endDestinationAction: PageStackAction.Pop;
    }
}
