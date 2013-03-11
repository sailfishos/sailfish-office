import QtQuick 1.1
import Sailfish.Silica 1.0

Rectangle {
    anchors.fill: parent
    color: "black"
    
    Label {
        id: label
        anchors.centerIn: parent
        text: "My Cover"
    }
    
    CoverActionList {
        id: coverAction
        
        CoverAction {
            iconSource: "image://theme/icon-cover-next"
        }
        
        CoverAction {
            iconSource: "image://theme/icon-cover-pause"
        }
    }
}


