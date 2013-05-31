import QtQuick 1.1
import Sailfish.Silica 1.0
import Sailfish.Office.Dropbox 1.0

Page {
    anchors.fill: parent;
    PageHeader { 
        id: header;
        title: "DropBox";
    }
    BackgroundItem {
        anchors {
            left: parent.left;
            right: parent.right;
            top: header.bottom;
            margins: theme.paddingSmall;
        }
        height: theme.itemSizeLarge;

        Label {
            anchors {
                left: parent.left;
                leftMargin: theme.paddingSmall;
                right: parent.right;
                top: parent.top;
                topMargin: theme.paddingSmall;
            }
            text: controllerMIT.needAuthenticate ? "Log in to DropBox..." : "Log out of DropBox";
            elide: Text.ElideRight;
            font.pixelSize: theme.fontSizeLarge;
        }
        MouseArea {
            anchors.fill: parent;
            onClicked: {
                if(controllerMIT.needAuthenticate) {
                    pageStack.push( loginPage );
                }
                else {
                    signOutNow();
                }
            }
        }
    }
    Component {
        id: loginPage;
        LoginPage { }
    }
    Page {
        id: signoutconfirmationDlg
        PageHeader { title: "Log Out" }
        Item {
            anchors.fill: parent;

            Text {
                anchors {
                    bottom: signoutButtonsRow.top;
                    left: parent.left;
                    right: parent.right;
                    margins: theme.paddingLarge;
                }
                color: "white"
                font.pixelSize: theme.fontSizeSmall
                font.family: theme.fontFamily
                wrapMode: Text.Wrap
                text: "Do you really want to log out of DropBox?"
            }
            Row {
                id: signoutButtonsRow;
                anchors.centerIn: parent;
                width: childrenRect.width;
                Button {
                    text: "Yes"
                    onPressed: {
                        controllerMIT.logout()
                        pageStack.pop()
                    }
                }
                Button {
                    text: "No"
                    onPressed: {
                        pageStack.pop()
                    }
                }
            }
        }
    }

    function signOutNow(){
        if (!controllerMIT.is_transfer()){
            pageStack.push(signoutconfirmationDlg)
        }else{
            i_infobanner.text = "Please complete the upload/download tasks in Files Transfer before Sign out."
            i_infobanner.visible = true
        }
    }
}
