import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

Page {
    id: page

    property string title;
    property string path;
    property string mimeType;

    allowedOrientations: window.allowedOrientations

    Drawer {
        id: drawer;

        anchors.fill: parent
        dock: page.orientation == Orientation.Portrait || page.orientation == Orientation.InvertedPortrait
                ? Dock.Top
                : Dock.Left

        background: DocumentsSharingList {
            visualParent: page;
            title: page.title;
            path: page.path;
            mimeType: page.mimeType;
            anchors.fill: parent;
        }


        Calligra.SpreadsheetCanvas {
            id: document;
            anchors.fill: parent;
        }

        SilicaFlickable {
            id: flickable

            anchors.fill: parent;

            Calligra.CanvasControllerItem {
                id: canvasController;
                canvas: document;
                flickable: flickable;

                minimumZoom: 1.0;
                maximumZoom: 2.5;
            }

            children: [
                HorizontalScrollDecorator { color: theme.highlightDimmerColor; },
                VerticalScrollDecorator { color: theme.highlightDimmerColor; }
            ]

            PinchArea {
                anchors.fill: parent;
                onPinchStarted: canvasController.beginZoomGesture();
                onPinchUpdated: canvasController.zoomBy(pinch.scale - pinch.previousScale, pinch.center);
                onPinchFinished: { canvasController.endZoomGesture(); flickable.returnToBounds(); }

                Calligra.LinkArea {
                    anchors.fill: parent;
                    onClicked: drawer.open = !drawer.open;
                }
            }
        }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            document.source = page.path;

            if(pageStack.nextPage(page) === null) {
                pageStack.pushAttached(Qt.resolvedUrl("SpreadsheetListPage.qml"), { title: page.title, canvas: document } );
            }
        }

        if(status == PageStatus.Activating) {
            flickable.contentX = 0;
            flickable.contentY = 0;
        }
    }
}
