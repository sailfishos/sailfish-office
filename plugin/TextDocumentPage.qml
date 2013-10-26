import QtQuick 2.0
import Sailfish.Silica 1.0
import org.kde.calligra 1.0 as Calligra

DocumentPage {
    id: page

    attachedPage: Component {
        TextDocumentToCPage {
            document: doc;
        }
    }

    Calligra.View {
        id: v;
        width: parent.width;
        height: parent.height;
        document: doc;
        SilicaFlickable {
            id: f
            anchors.fill: parent;

//             children: [
//                 HorizontalScrollDecorator {/* color: theme.highlightDimmerColor; */},
//                 VerticalScrollDecorator {/* color: theme.highlightDimmerColor; */}
//             ]

            Calligra.ViewController {
                view: v;
                flickable: f;
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: page.open = !page.open;
            }
        }

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

    Calligra.Document {
        id: doc;
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            doc.source = page.path;
        }
    }
}
