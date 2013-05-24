import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

Page {
    id: page;

    property string title;
    property string path;
    property string mimeType;

    allowedOrientations: Orientation.All;

    Drawer {
        id: drawer;

        anchors.fill: parent;
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

        SlideshowView {
            id: view;
            anchors.fill: parent;

            property Item currentItem;
            interactive: (currentItem === undefined) ? false : !currentItem.scaled;

            model: Calligra.PresentationModel {
                id: presentationModel;

                canvas: document;
                thumbnailSize.width: view.width;
                thumbnailSize.height: view.width * 0.75;
            }

            delegate: ZoomableThumbnail {
                width: view.width;
                height: Math.min(itemHeight, view.height);
                content: model.thumbnail;

                Calligra.LinkArea {
                    anchors.fill: parent;
                    onClicked: drawer.open = !drawer.open;
                }
            }

            Connections {
                target: view.currentItem;
                onUpdateSize: presentationModel.thumbnailSize = Qt.size(newWidth, newHeight);
            }

            onCurrentIndexChanged: presentationModel.thumbnailSize = Qt.size(view.width, view.width * 0.75);
        }

        Calligra.PresentationCanvas {
            id: document;
            anchors.fill: parent;
            visible: false;
        }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            document.source = page.path;

            if(pageStack.nextPage(page) === null) {
                pageStack.pushAttached(thumbnailPage, { title: page.title, canvas: document, view: view } );
            }
        }
    }

    Component {
        id: thumbnailPage;
        PresentationThumbnailPage { }
    }
}
