import QtQuick 1.1
import Sailfish.Silica 1.0

Page {
    id: page
    backNavigation: header.opacity > 0;
    
    Rectangle {
        id: rect;
        anchors.fill: parent;
        color: "grey";
        
//         SilicaFlickable {
//             id: document;
//             contentHeight: childrenRect.height;
//             
//         }
        
//         ScrollDecorator { flickable: document; }
        
        Rectangle {
            color: "white";
            x: page.width * 0.1;
            y: page.height * 0.1;
            width: page.width * 0.8;
            height: page.height * 0.8;
        }
        
        MouseArea {
            anchors.fill: parent;
            onClicked: { header.opacity = 1; fullScreenTimer.restart(); }
        }
    }
    
    Rectangle {
        id: header;
        
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: childrenRect.height;
        
        Behavior on opacity { NumberAnimation { } }
        color: "#000055";
        
        PageHeader {
            title: "Document";
        }
    }
    
    Timer {
        id: fullScreenTimer;
        interval: 5000;
        running: true;
        repeat: false;
        
        onTriggered: header.opacity = 0;
    }
}
