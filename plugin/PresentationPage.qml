import QtQuick 2.0
import Sailfish.Silica 1.0
import org.kde.calligra 1.0 as Calligra

DocumentPage {
    id: page;

    attachedPage: Component {
        PresentationThumbnailPage {
            document: doc;
        }
    }

    Calligra.Document {
        id: doc;
    }

    Calligra.View {
        id: v;
        anchors.fill: parent;
        document: doc;
    }

    SilicaFlickable {
        id: f;
        anchors.fill: parent;

//         children: [
//             HorizontalScrollDecorator { color: theme.highlightDimmerColor; },
//             VerticalScrollDecorator { color: theme.highlightDimmerColor; }
//         ]

        Calligra.ViewController {
            view: v;
            flickable: f;
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: page.open = !page.open;
        }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            doc.source = page.path;
        }
    }
}
