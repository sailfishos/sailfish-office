import QtQuick 1.1
import Sailfish.Silica 1.0
import Sailfish.Office 1.0

Page {
    id: page
    property alias model: listView.model;
    property string title: "Documents"

    SilicaListView {
        id: listView;
        anchors.fill: parent
        
        PullDownMenu {
            MenuItem {
                text: "Action";
                onClicked: console.log("Hello World!");
            }
        }
        
        ScrollDecorator { flickable: parent; }
        header: PageHeader { title: page.title; }
        //model: DocumentListModel { path: "/home/nemo/Documents"; }
        
        spacing: theme.paddingLarge;
        
        delegate: BackgroundItem {
            Label {
                anchors {
                    left: parent.left;
                    leftMargin: theme.paddingLarge;
                    top: parent.top;
                    topMargin: theme.paddingSmall;
                }
                text: model.fileName;
                
                font.pixelSize: theme.fontSizeLarge;
            }
            Label {
                anchors {
                    left: parent.left;
                    leftMargin: theme.paddingLarge;
                    bottom: parent.bottom;
                    bottomMargin: theme.paddingSmall;
                }
                text: model.fileType;
                
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
                
                text: model.fileSize;
                
                font.pixelSize: theme.fontSizeSmall;
                color: theme.secondaryColor;
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    switch(model.fileType) {
                        case "odt":
                            pageStack.push(Qt.resolvedUrl("TextDocumentPage.qml"), { title: model.fileName, path: model.filePath });
                        case "ods":
                            pageStack.push(Qt.resolvedUrl("SpreadsheetPage.qml"), { title: model.fileName, path: model.filePath });
                        case "odp":
                            pageStack.push(Qt.resolvedUrl("PresentationPage.qml"), { title: model.fileName, path: model.filePath });
                    }
                }
            }
        }
    }
}
