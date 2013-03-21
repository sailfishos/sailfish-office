import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

Page {
    id: page
    backNavigation: header.opacity > 0;
    
    property string title;
    property string path;
    
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
