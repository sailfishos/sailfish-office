import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Office 1.0
import Sailfish.Office.Files 1.0

Page {
    id: page
    property alias model: listView.model;
    property string title: "";

    allowedOrientations: Orientation.All;

    QtObject {
        id: theme;
        property int paddingLarge: 8;
        property int fontSizeLarge: 26;
        property int fontSizeSmall: 16;
        property color primaryColor: "black";
        property color secondaryColor: "silver";
        property color highlightColor: "white";
    }

    SilicaListView {
        id: listView;
        anchors.fill: parent
        
        /*PullDownMenu {
            MenuItem {
                text: "Action";
                onClicked: console.log("Hello World!");
            }
        }*/
        
        children: ScrollDecorator { }
        header: PageHeader { title: page.title; }
        //model: DocumentListModel { path: "/home/nemo/Documents"; }
        
        spacing: theme.paddingLarge;
        
        delegate: BackgroundItem {
            width: ListView.view.width;
            height: theme.itemSizeLarge;

            Image {
                id: icon;
                anchors {
                    left: parent.left;
                    top: parent.top;
                    topMargin: theme.paddingSmall;
                }

                source: "image://theme/icon-l-document"
            }

            Label {
                anchors {
                    left: icon.right;
                    leftMargin: theme.paddingSmall;
                    right: parent.right;
                    top: parent.top;
                    topMargin: theme.paddingSmall;
                }
                text: model.fileName;

                elide: Text.ElideRight;
                
                font.pixelSize: theme.fontSizeLarge;
            }
            Label {
                anchors {
                    left: icon.right;
                    leftMargin: theme.paddingSmall;
                    bottom: parent.bottom;
                    bottomMargin: theme.paddingSmall;
                }
                text: model.fileSize;

                font.pixelSize: theme.fontSizeSmall;
                color: theme.secondaryColor;
            }
            Label {
                anchors {
                    right: parent.right;
                    rightMargin: theme.paddingLarge;
                    bottom: parent.bottom;
                    bottomMargin: theme.paddingSmall;
                }
                
                text: Qt.formatDate(model.fileRead);
                
                font.pixelSize: theme.fontSizeSmall;
                color: theme.secondaryColor;
            }
            MouseArea {
                anchors.fill: parent;
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

//    Component {
//        id: pdfPage;
//        PDFDocumentPage { }
//    }
}
