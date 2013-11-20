import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Office.PDF 1.0 as PDF

SilicaFlickable {
    id: base;

    contentWidth: pdfCanvas.width;
    contentHeight: pdfCanvas.height;

    property alias itemWidth: pdfCanvas.width;
    property alias itemHeight: pdfCanvas.height;
    property alias document: pdfCanvas.document;

    property bool scaled: false;

    signal clicked();
    signal updateSize(real newWidth, real newHeight);

    function zoom(amount, center) {
        var oldWidth = pdfCanvas.width;

        pdfCanvas.width *= amount;

        if(pdfCanvas.width < d.minWidth) {
            pdfCanvas.width = d.minWidth;
        }

        if(pdfCanvas.width > d.maxWidth) {
            pdfCanvas.width = d.maxWidth;
        }

        if(pdfCanvas.width == d.minWidth) {
            base.scaled = false;
        } else {
            base.scaled = true;
        }

        var realZoom = pdfCanvas.width / oldWidth;
        contentX += (center.x * realZoom) - center.x;
        contentY += (center.y * realZoom) - center.y;
    }

    PDF.Canvas {
        id: pdfCanvas;

        width: base.width;
        spacing: Theme.paddingLarge;
        flickable: base;
        linkColor: Theme.highlightColor;

        PinchArea {
            anchors.fill: parent;
            onPinchUpdated: {
                var newCenter = mapToItem(pdfCanvas, pinch.center.x, pinch.center.y)
                base.zoom(1.0 + (pinch.scale - pinch.previousScale), newCenter);
            }
            onPinchFinished: base.returnToBounds();

            PDF.LinkArea {
                anchors.fill: parent;

                canvas: pdfCanvas;
                onLinkClicked: Qt.openUrlExternally(linkTarget);
                onClicked: base.clicked();
            }
        }
    }

    children: [
        HorizontalScrollDecorator { color: Theme.highlightDimmerColor; },
        VerticalScrollDecorator { color: Theme.highlightDimmerColor; }
    ]

    QtObject {
        id: d;

        property real minWidth: base.width;
        property real maxWidth: base.width * 2.5;
    }

    function goToPage(pageNumber) {
        base.contentY = pdfCanvas.pagePosition( pageNumber );
    }
}
