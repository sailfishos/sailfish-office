import QtQuick 1.1
import Sailfish.Silica 1.0
import com.jolla.components.views 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

SplitViewPage {
    id: page

    property string title;
    property string path;
    property string mimeType;

    allowedOrientations: Orientation.All;

    DocumentsSharingList {
        visualParent: page;
        title: page.title;
        path: page.path;
        mimeType: page.mimeType;
    }

    contentItem: Item {
        clip: true;

        Calligra.TextDocumentCanvas {
            id: document;
            anchors.fill: parent;
        }

        SilicaFlickable {
            id: aFlickable

            anchors.fill: parent;

            Calligra.CanvasControllerItem {
                id: canvasController;
                canvas: document;
                flickable: aFlickable;
            }

            children: ScrollDecorator { }

            PinchArea {
                anchors.fill: parent;
                onPinchStarted: canvasController.beginZoomGesture();
                onPinchUpdated: canvasController.zoomBy(pinch.scale - pinch.previousScale, pinch.center);
                onPinchFinished: { canvasController.endZoomGesture(); aFlickable.returnToBounds(); }

                MouseArea { anchors.fill: parent; onClicked: page.toggleSplit(); }
            }
        }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            document.source = page.path;

            if(pageStack.nextPage(page) === null) {
                pageStack.pushAttached(Qt.resolvedUrl("TextDocumentToCPage.qml"), { title: page.title, canvas: document } );
            }
        }
    }
}