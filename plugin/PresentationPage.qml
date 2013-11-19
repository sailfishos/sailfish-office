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

    SlideshowView {
        anchors.fill: parent;
        itemWidth: page.width;
        itemHeight: page.height;

        currentIndex: doc.currentIndex;

        model: Calligra.ContentsModel {
            document: doc;
            thumbnailSize.width: page.width;
            thumbnailSize.height: page.width * 0.75;
        }

        delegate: ZoomableThumbnail {
            width: PathView.view.itemWidth;
            maxHeight: PathView.view.itemHeight;

            y: - height / 2;

            content: PathView.view.model.thumbnail(index, page.width);

            onClicked: page.open = !page.open;

            onUpdateSize: {
                content = PathView.view.model.thumbnail(index, newWidth);
            }
        }
    }

    Calligra.Document {
        id: doc;
    }
    
    busy: doc.status != Calligra.DocumentStatus.Loaded;
    source: doc.source;
    indexCount: doc.indexCount;

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            doc.source = page.path;
        }
    }
}
