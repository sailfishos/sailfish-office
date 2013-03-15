import QtQuick 1.1
import Sailfish.Silica 1.0
import Sailfish.Office 1.0

Page {
    id: page
    
    SilicaListView {
        anchors.fill: parent
        
        PullDownMenu {
            MenuItem {
                text: "Action";
                onClicked: console.log("Hello World!");
            }
        }
        
        ScrollDecorator { flickable: parent; }
        header: PageHeader { title: "Documents"; }
        model: DocumentListModel { path: "/home/nemo/Documents"; }
        
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
                onClicked: pageStack.push(Qt.resolvedUrl("DocumentPage.qml"), { title: model.fileName, path: model.filePath });
            }
        }
    }
}
