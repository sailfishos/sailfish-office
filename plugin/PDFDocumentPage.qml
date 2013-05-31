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

        onWidthChanged: if( pdfCanvas.width < width ) pdfCanvas.width = width;

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

                if(newWidth >= view.width && newWidth <= Math.min(view.width, view.height) * 2.5)
                {
                    viewScale.xScale = pinch.scale;
                    viewScale.yScale = pinch.scale;
                }
            }

            onPinchFinished: {
                var oldScale = pdfCanvas.width / view.width;
                pdfCanvas.width *= viewScale.xScale;
                var newScale = pdfCanvas.width / view.width;

                var viewCenter = mapToItem( view, pinch.center.x, pinch.center.y );

                var xoff = (viewCenter.x + view.contentX) * newScale / oldScale;
                view.contentX = xoff - viewCenter.x;

                var yoff = (viewCenter.y + view.contentY) * newScale / oldScale;
                view.contentY = yoff - viewCenter.y;

                viewScale.xScale = 1;
                viewScale.yScale = 1;
                viewScale.origin.x = 0;
                viewScale.origin.y = 0;
            }

            Calligra.LinkArea {
                id: linkArea;
                anchors.fill: parent;
                linkColor: theme.highlightColor;
                onClicked: base.open = !base.open;
            }
        }
    }

    PDF.Document {
        id: pdfDocument;
        source: base.path;

        onLinkTargetsChanged: {
            linkArea.links = linkTargets;
            updateSourceSizeTimer.restart();
        }
    }

    Timer {
        id: updateSourceSizeTimer;
        interval: 5000;
        onTriggered: linkArea.sourceSize = Qt.size( base.width, pdfCanvas.height );
    }
}
