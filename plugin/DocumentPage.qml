import QtQuick 2.0
import Sailfish.Silica 1.0
import org.kde.calligra 1.0 as Calligra

Page {
    id: base;

    property variant attachedPage;

    property alias title: shareList.title;
    property alias path: shareList.path;
    property alias mimeType: shareList.mimeType;

    default property alias foreground: drawer.data;

    property alias open: drawer.open;
    property alias menu: shareList.children;
    property alias busy: busyIndicator.running;

    property url source;
    property int indexCount;

    allowedOrientations: Orientation.All;
    backNavigation: drawer.opened;
    forwardNavigation: drawer.opened;

    BusyIndicator { id: busyIndicator; anchors.centerIn: parent; size: BusyIndicatorSize.Large; }

    Component.onDestruction: window.documentItem = null
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

            PullDownMenu {
                MenuItem {
                    //: Show the Details page
                    //% "Details"
                    text: qsTrId("sailfish-office-me-details_page");
                    onClicked: pageStack.push(detailsPage, { source: base.source, indexCount: base.indexCount });
                }
            }
        }
    }

    onStatusChanged: {
        if( status == PageStatus.Active )
        {
            drawer.open = false;
            window.documentItem = drawer.foregroundItem;
        }
    }

    Component {
        id: detailsPage;

        DetailsPage { }
    }
}
