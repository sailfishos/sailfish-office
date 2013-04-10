import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

SilicaFlickable {
    id: base;

    contentWidth: thumb.width;
    contentHeight: thumb.height;

    property alias itemWidth: thumb.width;
    property alias itemHeight: thumb.height;
    property alias content: thumb.content;

    property bool scaled: false;

    signal clicked();
    signal updateSize(real newWidth, real newHeight);

    clip: true;

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

        if(thumb.width == d.minWidth) {
            base.scaled = false;
        } else {
            base.scaled = true;
        }

        contentX += (center.x * thumb.width / oldWidth) - center.x;
        if (thumb.height > height) {
            contentY += (center.y * thumb.height / oldHeight) - center.y;
        }
    }

    Calligra.Thumbnail {
        id: thumb;

        width: base.width;
        height: width * 0.75;
    }

    PinchArea {
        anchors.fill: parent;
        onPinchUpdated: parent.zoom(pinch.scale, pinch.center);

        MouseArea {
            anchors.fill: parent;
            onClicked: base.clicked();
        }
    }

    /**
     * The following is a workaround for missing currentItem in
     * QML's PathView.
     */
    Component.onCompleted: {
        if (PathView.isCurrentItem) {
            PathView.view.currentItem = base;
        }
    }

    PathView.onIsCurrentItemChanged: {
        if (PathView.isCurrentItem) {
            PathView.view.currentItem = base;
        }
    }

    QtObject {
        id: d;

        property real minWidth: base.width;
        property real maxWidth: base.width * 3;
    }

    Timer {
        id: updateTimer;

        interval: 500;
        repeat: false;
        onTriggered: base.updateSize(thumb.width, thumb.height);
    }
}
