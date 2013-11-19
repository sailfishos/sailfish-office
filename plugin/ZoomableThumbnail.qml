import QtQuick 2.0
import Sailfish.Silica 1.0
import org.kde.calligra 1.0 as Calligra

SilicaFlickable {
    id: base;

    property real maxHeight: height;

    height: Math.min(thumb.height, maxHeight);

    contentWidth: thumb.width;
    contentHeight: thumb.height;

    property alias content: thumb.data;
    property bool scaled: false;

    clip: true;

    signal clicked();
    signal updateSize(real newWidth, real newHeight);

    function zoom(amount, center) {

        var oldWidth = thumb.width;
        var oldHeight = thumb.height;

        thumb.width *= amount;
        updateTimer.restart();

        if(thumb.width < d.minWidth) {
            thumb.width = d.minWidth;
        }

        if(thumb.width > d.maxWidth) {
            thumb.width = d.maxWidth;
        }

        if(Math.abs(thumb.width - d.minWidth) < 5 ) {
            base.scaled = false;
        } else {
            base.scaled = true;
        }

        var realZoom = thumb.width / oldWidth;
        thumb.height *= realZoom;

        contentX += (center.x * thumb.width / oldWidth) - center.x;
        if (thumb.height > height) {
            contentY += (center.y * thumb.height / oldHeight) - center.y;
        }
    }

    Calligra.ImageDataItem {
        id: thumb;

        children: [
            PinchArea {
                anchors.fill: parent;
                onPinchUpdated: base.zoom(1.0 + (pinch.scale - pinch.previousScale), pinch.center);
                onPinchFinished: base.returnToBounds();

                MouseArea {
                    anchors.fill: parent;
                    onClicked: base.clicked();
                }
            }
        ]
    }

    QtObject {
        id: d;

        property real minWidth: base.width;
        property real maxWidth: base.width * 2.5;
    }

    Timer {
        id: updateTimer;

        interval: 500;
        repeat: false;
        onTriggered: base.updateSize(thumb.width, thumb.height);
    }
}
