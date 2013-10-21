import QtQuick 2.0
import Sailfish.Silica 1.0
import org.kde.calligra 1.0 as Calligra

DocumentPage {
    id: page;

    attachedPage: Component {
        PresentationThumbnailPage {
            canvas: document;
            view: v;
        }
    }

    Calligra.Document {
        id: document;
    }

    Calligra.View {
        id: v;
        anchors.fill: parent;
        document: page.document;
    }

    Flickable {
        id: f;
        anchors.fill: parent;

        Calligra.ViewController {
            view: v;
            flickable: f;
        }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            document.source = page.path;
        }
    }
}
