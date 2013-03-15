import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

Page {
    id: page
    backNavigation: header.opacity > 0;
    
    property string title;
    property string path;
    
    Component.onCompleted: console.log( "Opening " + path );
    
//     Rectangle {
//         id: rect;
//         anchors.fill: parent;
//         color: "grey";
//         
// //         SilicaFlickable {
// //             id: document;
// //             contentHeight: childrenRect.height;
// //             
// //         }
//         
// //         ScrollDecorator { flickable: document; }
//         
//         Rectangle {
//             color: "white";
//             x: page.width * 0.1;
//             y: page.height * 0.1;
//             width: page.width * 0.8;
//             height: page.height * 0.8;
//         }
//         
//         MouseArea {
//             anchors.fill: parent;
//             onClicked: { header.opacity = 1; fullScreenTimer.restart(); }
//         }
//     }
    Calligra.TextDocumentCanvas {
        id: document;
        anchors.fill: parent;
        source: page.path;
        zoomMode: Calligra.TextDocumentCanvas.ZOOM_WIDTH;
    }
    
    MouseArea {
        anchors.fill: parent;
        onClicked: { header.opacity = 1; fullScreenTimer.restart(); }
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
        color: "black";
        
        PageHeader {
            title: page.title;
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
