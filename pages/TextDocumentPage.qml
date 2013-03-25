import QtQuick 1.1
import Sailfish.Silica 1.0
import Sailfish.TransferEngine 1.0
import com.jolla.components.views 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

SplitViewPage {
    id: page
    
    property string title;
    property string path;

    ShareMethodList {
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            bottom: parent.verticalCenter;
        }

        header: PageHeader { title: page.title; }

        PullDownMenu {
            MenuItem {
                text: "Select";
            }
        }

        model: SailfishTransferMethodsModel { }
        source: page.path;
    }

    contentItem: Rectangle {
        color: "grey";
        clip: true;
        SilicaFlickable {
            id: flickable

            anchors.fill: parent;

            contentWidth: document.documentSize.width;
            contentHeight: document.documentSize.height;
            clip: true;

            Calligra.TextDocumentCanvas {
                id: document;
                width: page.width;
                height: page.height;
                zoomMode: Calligra.TextDocumentCanvas.ZOOM_PAGE;
            }

            ScrollDecorator { flickable: parent; }
            MouseArea { anchors.fill: parent; onClicked: page.splitActive = !page.splitActive; }
        }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            document.source = page.path;
        }
    }
}
