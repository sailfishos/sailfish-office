import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

Page {
    id: page

    property string title;
    property string path;

    Rectangle {
        anchors.fill: parent;
        color: "grey";
    }

    SilicaFlickable {
        anchors.fill: parent;

        contentWidth: document.width;
        contentHeight: document.height;
        clip: true;

        Calligra.SpreadsheetCanvas {
            id: document;
            width: page.width * 2;
            height: page.height * 2;
        }

        ScrollDecorator { flickable: parent; }
        MouseArea { anchors.fill: parent; onClicked: mouse.accept(); }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            document.source = page.path;
        }
    }
}
