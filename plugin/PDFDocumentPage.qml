import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Office.PDF 1.0 as PDF

DocumentPage {
    id: base;

    attachedPage: Component {
        PDFDocumentToCPage {
            tocModel: pdfDocument.tocModel
            onPageSelected: view.contentY = pdfCanvas.pagePosition( pageNumber );
        }
    }

    PDFView {
        id: view;
        anchors.fill: parent;
        document: pdfDocument;

        onClicked: base.open = !base.open;
    }

    PDF.Document {
        id: pdfDocument;
        source: base.path;
    }

    busy: !pdfDocument.loaded;
    source: pdfDocument.source;
    indexCount: pdfDocument.pageCount;

    Timer {
        id: updateSourceSizeTimer;
        interval: 5000;
        onTriggered: linkArea.sourceSize = Qt.size( base.width, pdfCanvas.height );
    }
}
