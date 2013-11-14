import QtQuick 2.0
import Sailfish.Office 1.0

//TODO: This really should be in plugin/ but also should be a bit
//cleaner so someone can simply instantiate an object and call a
//method on that to open a document.
Item {
    property alias textDocument: textDocumentPage;
    property alias spreadsheet: spreadsheetPage;
    property alias presentation: presentationPage;
    property alias pdf: pdfPage;

    Component {
        id: textDocumentPage;
        TextDocumentPage { }
    }

    Component {
        id: spreadsheetPage;
        SpreadsheetPage { }
    }

    Component {
        id: presentationPage;
        PresentationPage { }
    }

    Component {
        id: pdfPage;
        PDFDocumentPage { }
    }
}
