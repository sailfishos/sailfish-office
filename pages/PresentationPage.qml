import QtQuick 1.1
import Sailfish.Silica 1.0
import com.jolla.components.views 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

SplitViewPage {
    id: page;

    property string title;
    property string path;
    property string mimeType;

    PageHeader { anchors.right: parent.right; title: page.title; }

    contentItem: Rectangle {
        color: "grey";

        Calligra.PresentationCanvas {
            id: document;
            anchors.fill: parent;
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
            MouseArea { anchors.fill: parent; onClicked: page.toggleSplit(); }
        }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            document.source = page.path;
        }
    }
}
