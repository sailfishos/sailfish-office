import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Office.PDF 1.0 as PDF

SilicaFlickable {
    id: base;

    contentWidth: canvas.width;
    contentHeight: canvas.height;

    property alias itemWidth: canvas.width;
    property alias itemHeight: canvas.height;
    property alias document: canvas.document;

    property bool scaled: false;

    signal clicked();
    signal updateSize(real newWidth, real newHeight);

    function zoom(amount, center) {
        var oldWidth = canvas.width;

        canvas.width *= amount;

        if(canvas.width < d.minWidth) {
            canvas.width = d.minWidth;
        }

        if(canvas.width > d.maxWidth) {
            canvas.width = d.maxWidth;
        }

        if(canvas.width == d.minWidth) {
            base.scaled = false;
        } else {
            base.scaled = true;
        }

        var realZoom = canvas.width / oldWidth;
        contentX += (center.x * realZoom) - center.x;
        contentY += (center.y * realZoom) - center.y;
    }

    PDF.Canvas {
        id: canvas;

        width: base.width;
        spacing: Theme.paddingLarge;
        flickable: base;

        PinchArea {
            anchors.fill: parent;
            onPinchUpdated: {
                var newCenter = mapToItem(canvas, pinch.center.x, pinch.center.y)
                base.zoom(1.0 + (pinch.scale - pinch.previousScale), newCenter);
            }
            onPinchFinished: base.returnToBounds();
        }

        PDF.LinkArea {
            anchors.fill: parent;
            links: pdfDocument.linkTargets;
            onLinkClicked: Qt.openUrlExternally(linkTarget);
            onClicked: base.clicked();
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
        base.contentY = canvas.pagePosition( pageNumber );
    }
}
