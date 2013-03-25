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

        Calligra.TextDocumentCanvas {
            id: document;
            anchors.fill: parent;
            zoomMode: Calligra.TextDocumentCanvas.ZOOM_WIDTH;
        }

        SilicaFlickable {
            id: flickable

            anchors.fill: parent;

            contentWidth: canvasController.documentSize.width;
            contentHeight: canvasController.documentSize.height;

            Calligra.CanvasControllerItem {
                id: canvasController;
                canvas: document;
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
