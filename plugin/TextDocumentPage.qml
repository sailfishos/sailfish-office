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

            Calligra.ViewController {
                id: controller;
                view: v;
                flickable: f;
            }

            children: [
                HorizontalScrollDecorator { color: Theme.highlightDimmerColor; },
                VerticalScrollDecorator { color: Theme.highlightDimmerColor; }
            ]

            PinchArea {
                anchors.fill: parent;

                onPinchUpdated: {
                    var newCenter = mapToItem( f, pinch.center.x, pinch.center.y );
                    controller.zoom = pinch.scale;
                }
                onPinchFinished: { f.returnToBounds(); }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: page.open = !page.open;
                }
            }
        }
    }

    Calligra.Document {
        id: doc;
    }
    
    busy: doc.status != Calligra.DocumentStatus.Loaded;

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            doc.source = page.path;
        }
    }
}
