import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Office.Files 1.0

Page {
    id: root

    signal sortSelected(int sortType)

    SilicaListView {
        anchors.fill: parent
        model: sortModel

        header: PageHeader {
            //% "Sort by"
            title: qsTrId("jolla-email-he-sort_by")
        }

        delegate: BackgroundItem {
            Label {
                x: Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter
                text: name
                color: highlighted ? Theme.highlightColor : Theme.primaryColor
            }

            onClicked: root.sortSelected(sortType)
        }
        VerticalScrollDecorator {}
    }

    ListModel {
        id: sortModel

        ListElement {
            sortType: FilterModel.Name
            //: Sort by name
            //% "Name"
            name: qsTrId("jolla-email-me-sort_name")
        }

        ListElement {
            sortType: FilterModel.Type
            //: Sort by type
            //% "Type"
            name: qsTrId("jolla-email-me-sort_type")
        }

        ListElement {
            sortType: FilterModel.Date
            //: Sort by date
            //% "Date"
            name: qsTrId("jolla-email-me-sort_date")
        }
    }
}
