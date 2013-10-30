import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page;
    signal pageSelected(int pageNumber);

    property alias tocModel: tocListView.model;
    property int currentPageNumber: 0;

    allowedOrientations: Orientation.All;

    SilicaListView {
        id: tocListView
        anchors.fill: parent;

        //: Page with PDF index
        //% "Index"
        header: PageHeader { title: qsTrId( "sailfish-office-he-pdf_index" ); }

        delegate: BackgroundItem {
            //highlighted: model.page == page.canvas.currentPageNumber;

            Label {
                anchors {
                    left: parent.left;
                    leftMargin: Theme.paddingLarge * model.level;
                    right: pageNumberLbl.left;
                    rightMargin: Theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                elide: Text.ElideRight;
                text: (model.title === undefined) ? "" : model.title;
            }
            Label {
                id: pageNumberLbl
                anchors {
                    right: parent.right;
                    rightMargin: Theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                text: (model.pageNumber === undefined) ? "" : model.pageNumber;
            }

            onClicked: {
                page.pageSelected(model.pageNumber);
                pageStack.navigateBack(PageStackAction.Animated);
            }
        }
    }
}
