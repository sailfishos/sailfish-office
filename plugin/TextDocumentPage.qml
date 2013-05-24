import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

DocumentPage {
    id: page

    attachedPage: Component {
        TextDocumentToCPage {
            canvas: document;
        }
    }

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

            minimumZoom: -1.0; //Fit to flickable
            maximumZoom: 2.5;
        }

        children: [
            HorizontalScrollDecorator { color: theme.highlightDimmerColor; },
            VerticalScrollDecorator { color: theme.highlightDimmerColor; }
        ]

        PinchArea {
            anchors.fill: parent;
            onPinchStarted: canvasController.beginZoomGesture();
            onPinchUpdated: {
                var newCenter = mapToItem( aFlickable, pinch.center.x, pinch.center.y );
                canvasController.zoomBy(pinch.scale - pinch.previousScale, Qt.point( newCenter.x, newCenter.y ) );
            }
            onPinchFinished: { canvasController.endZoomGesture(); aFlickable.returnToBounds(); }

            Calligra.LinkArea {
                anchors.fill: parent;
                links: document.linkTargets;
                onClicked: page.open = !page.open;
                onLinkClicked: Qt.openUrlExternally(linkTarget);
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
