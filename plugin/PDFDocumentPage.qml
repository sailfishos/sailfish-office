import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra
import Sailfish.Office.PDF 1.0 as PDF

DocumentPage {
    id: base;

    attachedPage: Component {
        PDFDocumentToCPage {
            tocModel: pdfDocument.tocModel
            onPageSelected: view.contentY = pdfCanvas.pagePosition( pageNumber );
        }
    }

    SilicaFlickable {
        id: view;

        anchors.fill: parent;

        contentWidth: pdfCanvas.width;
        contentHeight: pdfCanvas.height;

        transform: Scale { id: viewScale; }

        clip: true;

        PDF.Canvas {
            id: pdfCanvas;
            document: pdfDocument;
            width: base.width;
        }

        children: [
            HorizontalScrollDecorator { color: theme.highlightDimmerColor; },
            VerticalScrollDecorator { color: theme.highlightDimmerColor; }
        ]

        PinchArea {
            anchors.fill: parent;

            onPinchUpdated: {
                var viewCenter = mapToItem( view, pinch.center.x, pinch.center.y );
                viewScale.origin.x = viewCenter.x;
                viewScale.origin.y = viewCenter.y;

                var newWidth = pdfCanvas.width * pinch.scale;

                if(newWidth >= view.width && newWidth <= view.width * 2.5)
                {
                    viewScale.xScale = pinch.scale;
                    viewScale.yScale = pinch.scale;
                }
            }

            onPinchFinished: {
                var oldScale = odfCanvas.width / view.width;
                pdfCanvas.width *= viewScale.xScale;
                var newScale = pdfCanvas.width / view.width;

                var xoff = (view.width / 2 + view.contentX) * newScale / oldScale;
                view.contentX = xoff - view.width / 2;

                var yoff = (view.height / 2 + view.contentY) * newScale / oldScale;
                view.contentY = yoff - view.height /2;

                viewScale.xScale = 1;
                viewScale.yScale = 1;
                viewScale.origin.x = 0;
                viewScale.origin.y = 0;
            }

            Calligra.LinkArea {
                anchors.fill: parent;
                onClicked: base.open = !base.open;
                links: pdfDocument.linkTargets;
            }
        }
    }

    PDF.Document {
        id: pdfDocument;
        source: base.path;
    }
}

