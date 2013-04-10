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
    }

    contentItem: Item {
        clip: true;

        SlideshowView {
            id: view;
            anchors.fill: parent;

            property Item currentItem;
            interactive: !currentItem.scaled;

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

                onClicked: page.toggleSplit();
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
