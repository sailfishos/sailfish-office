import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

DocumentPage {
    id: page;

    attachedPage: Component {
        PresentationThumbnailPage {
            canvas: document;
            view: slideView;
        }
    }

    SlideshowView {
        id: slideView;
        anchors.fill: parent;

        property Item currentItem;
        interactive: (currentItem === undefined) ? false : !currentItem.scaled;

        model: Calligra.PresentationModel {
            id: presentationModel;

            canvas: document;
            thumbnailSize.width: slideView.width;
            thumbnailSize.height: slideView.width * 0.75;
        }

        delegate: ZoomableThumbnail {
            width: slideView.width;
            height: Math.min(itemHeight, slideView.height);
            content: model.thumbnail;

            Calligra.LinkArea {
                anchors.fill: parent;
                linkColor: theme.highlightColor;
                onClicked: page.open = !page.open;
            }
        }

        Connections {
            target: view.currentItem;
            onUpdateSize: presentationModel.thumbnailSize = Qt.size(newWidth, newHeight);
        }

        onCurrentIndexChanged: presentationModel.thumbnailSize = Qt.size(slideView.width, slideView.width * 0.75);
    }

    Calligra.PresentationCanvas {
        id: document;
        anchors.fill: parent;
        visible: false;
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            document.source = page.path;
        }
    }
}
