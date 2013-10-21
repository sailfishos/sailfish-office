import QtQuick 2.0
import Sailfish.Silica 1.0
import org.kde.calligra 1.0 as Calligra

DocumentPage {
    id: page

//     attachedPage: Component {
//         TextDocumentToCPage {
//             canvas: theDoc;
//             onClicked: aFlickable.contentY = theDoc.pagePosition( page );
//         }
//     }

    Calligra.Document {
        id: theDoc;
        //source: doc
    }
    Calligra.View {
        id: v;
        anchors.fill: parent;
        document: theDoc;
    }
//     Calligra.TextDocumentCanvas {
//         id: document;
//         anchors.fill: parent;
// 
//         onLinkTargetsChanged: {
//             linkArea.links = linkTargets;
//             linkArea.sourceSize = canvasController.documentSize;
//         }
//     }

    SilicaFlickable {
        id: aFlickable
        anchors.fill: parent;

        //onWidthChanged: if( canvasController.documentSize.width < width ) canvasController.fitToWidth( width );

        Calligra.ViewController {
            view: v;
            flickable: aFlickable;
        }

        children: [
            HorizontalScrollDecorator {/* color: theme.highlightDimmerColor; */},
            VerticalScrollDecorator {/* color: theme.highlightDimmerColor; */}
        ]

//         PinchArea {
//             anchors.fill: parent;
//             onPinchStarted: canvasController.beginZoomGesture();
//             onPinchUpdated: {
//                 var newCenter = mapToItem( aFlickable, pinch.center.x, pinch.center.y );
//                 canvasController.zoomBy(pinch.scale - pinch.previousScale, Qt.point( newCenter.x, newCenter.y ) );
//             }
//             onPinchFinished: { canvasController.endZoomGesture(); aFlickable.returnToBounds(); }
// 
//             Calligra.LinkArea {
//                 id: linkArea;
//                 anchors.fill: parent;
//                 linkColor: theme.highlightColor;
// 
//                 onClicked: page.open = !page.open;
//                 onLinkClicked: Qt.openUrlExternally(linkTarget);
//             }
//         }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            theDoc.source = page.path;
        }
    }
}
