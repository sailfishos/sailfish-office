import QtQuick 2.0
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
            margins: Theme.paddingSmall;
        }
        height: Theme.itemSizeLarge;

        Button {
            anchors {
                left: parent.left;
                leftMargin: Theme.paddingLarge;
                right: parent.right;
                rightMargin: Theme.paddingLarge;
                top: parent.top;
                topMargin: Theme.paddingSmall;
            }
            text: controllerMIT.needAuthenticate ? "Log in to DropBox" : "Log out of DropBox";
            //elide: Text.ElideRight;
            //font.pixelSize: Theme.fontSizeLarge;
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
                    margins: Theme.paddingLarge;
                }
                color: "white"
                font.pixelSize: Theme.fontSizeSmall
                font.family: Theme.fontFamily
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
