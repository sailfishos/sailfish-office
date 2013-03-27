import QtQuick 1.1
import Sailfish.Silica 1.0
import Sailfish.TransferEngine 1.0
import com.jolla.components.accounts 1.0
import com.jolla.components.views 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

SplitViewPage {
    id: page

    property string title;
    property string path;
    property string mimeType;

    allowedOrientations: window.allowedOrientations

    SailfishTransferMethodsModel {
        id: transferMethodsModel;
        filter: page.mimeType;
    }
    ShareMethodList {
        id: menuList
        objectName: "menuList"
        model: transferMethodsModel;
        source: page.path;

        anchors.fill: {
            left: parent.left
            top: parent.top
            right: isPortrait ? parent.right : parent.horizontalCenter
            bottom: isPortrait ? parent.verticalCenter : parent.bottom
        }

/*        PullDownMenu {
            MenuItem {
                text: "Select";
            }
        }
*/
        //listHeader: "Share"

        header: Item {
            height: theme.itemSizeLarge
            width: menuList.width * 0.7 - theme.paddingLarge
            x: menuList.width * 0.3

            Label {
                text: page.title
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
/*        footer: BackgroundItem {
            Label {
                //% "Add account"
                text: "Add account"
                x: theme.paddingLarge
                anchors.verticalCenter: parent.verticalCenter
                color: parent.down ? theme.highlightColor : theme.primaryColor
                visible: true
            }

            onClicked: {
                jolla_signon_ui_service.inProcessParent = page
                pageStack.push(accountsPage)
            }
        }

        Component {
            id: accountsPage
            AccountsPage { }
        }
*/    }

    contentItem: Rectangle {
        color: "grey";
        clip: true;

        Calligra.TextDocumentCanvas {
            id: document;
            anchors.fill: parent;
            zoomMode: Calligra.TextDocumentCanvas.ZOOM_WIDTH;
        }

        SilicaFlickable {
            id: aFlickable

            anchors.fill: parent;

            Calligra.CanvasControllerItem {
                id: canvasController;
                canvas: document;
                flickable: aFlickable;
            }

            ScrollDecorator { flickable: parent; }

            PinchArea {
                anchors.fill: parent;
                onPinchUpdated: parent.zoom += pinch.scale - pinch.previousScale;

                MouseArea { anchors.fill: parent; onClicked: page.toggleSplit(); }
            }
        }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            document.source = page.path;
        }
    }
}
