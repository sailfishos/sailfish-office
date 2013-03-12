import QtQuick 1.1
import Sailfish.Silica 1.0

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
        
        header: PageHeader {
            title: "Documents";
        }
        
        model: ListModel {
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
            ListElement { name: "Document"; type: "OpenDocument Text"; size: "10KiB"; }
        }
        
        spacing: theme.paddingLarge;
        
        delegate: BackgroundItem {
            Label {
                anchors {
                    left: parent.left;
                    leftMargin: theme.paddingLarge;
                    top: parent.top;
                    topMargin: theme.paddingSmall;
                }
                text: model.name;
                
                font.pixelSize: theme.fontSizeLarge;
            }
            Label {
                anchors {
                    left: parent.left;
                    leftMargin: theme.paddingLarge;
                    bottom: parent.bottom;
                    bottomMargin: theme.paddingSmall;
                }
                text: model.type;
                
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
                
                text: model.size;
                
                font.pixelSize: theme.fontSizeSmall;
                color: theme.secondaryColor;
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: pageStack.push(Qt.resolvedUrl("DocumentPage.qml"));
            }
        }
    }
}
