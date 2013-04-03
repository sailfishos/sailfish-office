import QtQuick 1.1
import Sailfish.Silica 1.0
import Sailfish.TransferEngine 1.0
import com.jolla.components.accounts 1.0

ShareMethodList {
    id: menuList
    objectName: "menuList"
    property Item visualParent;
    property string path;
    property string mimeType;

    model: SailfishTransferMethodsModel {
        id: transferMethodsModel;
        filter: menuList.mimeType;
    }
    source: menuList.path;

    anchors {
        left: parent.left
        top: parent.top
        right: isPortrait ? parent.right : parent.horizontalCenter
        bottom: isPortrait ? parent.verticalCenter : parent.bottom
    }

    listHeader: "Share"

    header: Item {
        height: theme.itemSizeLarge
        width: menuList.width * 0.7 - theme.paddingLarge
        x: menuList.width * 0.3

        Label {
            text: visualParent.title
            width: parent.width
            truncationMode: TruncationMode.Fade
            color: theme.highlightColor
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignRight
            font {
                pixelSize: theme.fontSizeLarge
                family: theme.fontFamilyHeading
            }
        }
    }

    // Add "add account" to the footer. User must be able to
    // create accounts in a case there are none.
//     footer: BackgroundItem {
//         Label {
//             //% "Add account"
//             text: "Add account"
//             x: theme.paddingLarge
//             anchors.verticalCenter: parent.verticalCenter
//             color: parent.down ? theme.highlightColor : theme.primaryColor
//             visible: true
//         }
// 
//         onClicked: {
//             jolla_signon_ui_service.inProcessParent = visualParent
//             pageStack.push(accountsPage)
//         }
//     }
// 
//     Component {
//         id: accountsPage
//         AccountsPage { }
//     }
}