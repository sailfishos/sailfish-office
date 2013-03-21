import QtQuick 1.1
import Sailfish.Silica 1.0
import com.jolla.components.views 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

SplitViewPage {
    id: page;

    property string title;
    property string path;

    PageHeader { anchors.right: parent.right; title: page.title; }

    contentItem: Rectangle {
        color: "grey";

        SilicaFlickable {
            anchors.fill: parent;

            contentWidth: document.width;
            contentHeight: document.height;
            clip: true;

            Calligra.PresentationCanvas {
                id: document;
                width: page.width * 2;
                height: page.height;
            }

            ScrollDecorator { flickable: parent; }
            MouseArea { anchors.fill: parent; onClicked: page.splitActive = !page.splitActive; }
        }
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            document.source = page.path;
        }
    }
}
