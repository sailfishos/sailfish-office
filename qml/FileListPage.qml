import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Office 1.0
import Sailfish.Office.Files 1.0

Page {
    id: page
    property alias model: listView.model;
    property string title: "";

    allowedOrientations: Orientation.All;

    SilicaListView {
        id: listView;
        anchors.fill: parent
        
        children: ScrollDecorator { }
        header: PageHeader { title: page.title; }
        
        delegate: BackgroundItem {
            id: bg;
            width: ListView.view.width;

            HighlightImage {
                id: icon;
                anchors {
                    left: parent.left;
                    leftMargin: Theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                highlighted: bg.highlighted;
                source: "image://theme/icon-l-document"
            }

            Label {
                anchors {
                    left: icon.right;
                    leftMargin: Theme.paddingMedium;
                    right: parent.right;
                    bottom: icon.verticalCenter;
                }
                color: bg.highlighted ? Theme.highlightColor : Theme.primaryColor
                text: model.fileName;
                font.pixelSize: Theme.fontSizeMedium;
                truncationMode: TruncationMode.Fade;
            }
            Label {
                anchors {
                    left: icon.right;
                    leftMargin: Theme.paddingMedium;
                    top: icon.verticalCenter;
                }
                text: Format.formatFileSize(model.fileSize);

                font.pixelSize: Theme.fontSizeExtraSmall;
                color: Theme.secondaryColor;
                opacity: 0.6;
            }
            Label {
                anchors {
                    right: parent.right;
                    rightMargin: Theme.paddingLarge;
                    top: icon.verticalCenter;
                }
                
                text: Format.formatDate(model.fileRead, Format.Timepoint);
                
                font.pixelSize: Theme.fontSizeExtraSmall;
                color: Theme.secondaryColor;
                opacity: 0.6;
            }

            onClicked: {
                switch(model.fileDocumentClass) {
                    case DocumentListModel.TextDocument:
                        pageStack.push(textDocumentPage, { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType });
                        break;
                    case DocumentListModel.SpreadSheetDocument:
                        pageStack.push(spreadsheetPage, { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType });
                        break;
                    case DocumentListModel.PresentationDocument:
                        pageStack.push(presentationPage, { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType });
                        break;
                    case DocumentListModel.PDFDocument:
                        pageStack.push(pdfPage, { title: model.fileName, path: model.filePath, mimeType: model.fileMimeType });
                        break;
                    default:
                        console.log("Unknown file format for file " + model.fileName + " with stated mimetype " + model.fileMimeType);
                        break;
                }
            }
        }
    }

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
