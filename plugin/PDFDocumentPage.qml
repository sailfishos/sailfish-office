import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra
import Sailfish.Office.PDF 1.0 as PDF

Page {
    id: base;

    property string title;
    property string path;
    property string mimeType;

    allowedOrientations: Orientation.All;

    Drawer {
        id: drawer;

        anchors.fill: parent
        dock: base.orientation == Orientation.Portrait || base.orientation == Orientation.InvertedPortrait
                ? Dock.Top
                : Dock.Left

        background: DocumentsSharingList {
            visualParent: base;
            title: base.title;
            path: base.path;
            mimeType: base.mimeType;
            anchors.fill: parent;
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
                    viewScale.xScale = pinch.scale;
                    viewScale.yScale = pinch.scale;
                }

                onPinchFinished: {
                    var oldWidth = pdfCanvas.width;
                    var oldHeight = pdfCanvas.height;

                    var newWidth = pdfCanvas.width * viewScale.xScale;

                    if(newWidth >= view.width && newWidth <= view.width * 2.5)
                    {
                        pdfCanvas.width = newWidth;

                        view.contentX += (viewScale.origin.x * newWidth / oldWidth) - viewScale.origin.x;
                        view.contentY += (viewScale.origin.y * (oldHeight * viewScale.yScale) / oldHeight) - viewScale.origin.y;
                    }

                    viewScale.xScale = 1;
                    viewScale.yScale = 1;
                    viewScale.origin.x = 0;
                    viewScale.origin.y = 0;

                    view.returnToBounds();
                }

                Calligra.LinkArea {
                    anchors.fill: parent;
                    onClicked: drawer.open = !drawer.open;
                    links: pdfDocument.linkTargets;
                }
            }
        }
    }

    PDF.Document {
        id: pdfDocument;
        source: base.path;
    }

    onStatusChanged: {
        if(status == PageStatus.Active) {
            if(pageStack.nextPage(base) === null) {
                pageStack.pushAttached( tocPage );
            }
        }
    }
    Component {
        id: tocPage;
        PDFDocumentToCPage {
            title: base.title;
            tocModel: pdfDocument.tocModel
            onPageSelected: view.contentY = pdfCanvas.pagePosition( pageNumber );
        }
    }
}
