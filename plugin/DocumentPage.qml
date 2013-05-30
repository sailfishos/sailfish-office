import QtQuick 1.1
import Sailfish.Silica 1.0

Page {
    id: base;

    property variant attachedPage;

    property alias title: shareList.title;
    property alias path: shareList.path;
    property alias mimeType: shareList.mimeType;

    default property alias foreground: drawer.data;

    property alias open: drawer.open;

    allowedOrientations: Orientation.All;
    backNavigation: drawer.opened;
    forwardNavigation: drawer.opened;

    Drawer {
        id: drawer;

        anchors.fill: parent
        dock: base.orientation == Orientation.Portrait || base.orientation == Orientation.InvertedPortrait
                ? Dock.Top
                : Dock.Left

        hideOnMinimize: true;

        onOpenChanged: {
            if( open && pageStack.nextPage( base ) === null )
                pageStack.pushAttached( base.attachedPage );
        }

        background: DocumentsSharingList {
            id: shareList;

            visualParent: base;
            anchors.fill: parent;
        }
    }

    onStatusChanged: {
        if( status == PageStatus.Active )
        {
            drawer.open = false;
        }
    }
}
