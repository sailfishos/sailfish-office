import QtQuick 1.1
import Sailfish.Silica 1.0
import com.jolla.components.views 1.0
import org.calligra.CalligraComponents 0.1 as Calligra
import "../components"

SplitViewPage {
    id: page;

    property string title;
    property string path;
    property string mimeType;

    allowedOrientations: window.allowedOrientations

    DocumentsSharingList {
        visualParent: page;
        path: page.path;
        mimeType: page.mimeType;
        PullDownMenu {
            MenuItem {
                text: "Next Slide";
                onClicked: document.currentSlide++;
            }
            MenuItem {
                text: "Previous Slide";
                onClicked: document.currentSlide--;
            }
        }
    }

    contentItem: Rectangle {
        color: "grey";

        Calligra.PresentationCanvas {
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
            }

            ScrollDecorator { flickable: parent; }

            PinchArea {
                anchors.fill: parent;
                onPinchUpdated: canvasController.zoom += pinch.scale - pinch.previousScale;

                MouseArea { anchors.fill: parent; onClicked: page.toggleSplit(); }
            }
        }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            document.source = page.path;

            if(pageStack.nextPage(page) === null) {
                pageStack.pushAttached(thumbnailPage, { title: page.title, canvas: document } );
            }
        }
    }

    Component {
        id: thumbnailPage;
        PresentationThumbnailPage { }
    }
}
