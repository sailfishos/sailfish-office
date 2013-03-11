import QtQuick 1.1
import Sailfish.Silica 1.0

Page {
    id: page
    SilicaListView {
        id: listView
        model: 20
        anchors.fill: parent
        header: PageHeader {
            title: "Nested Page"
        }
        delegate: BackgroundItem {
            Label {
                x: theme.paddingLarge
                text: "Item " + index
            }
            onClicked: console.log("Clicked " + index)
        }
    }
}





