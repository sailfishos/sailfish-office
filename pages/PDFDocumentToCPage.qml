import QtQuick 1.1
import Sailfish.Silica 1.0

Page {
    id: page;

    property string title;
    property alias tocModel: tocListView.model;
    property int currentPageNumber: 0;

    SilicaListView {
        id: tocListView
        anchors.fill: parent;

        header: PageHeader { title: page.title; }

        delegate: BackgroundItem {
            //highlighted: model.page == page.canvas.currentPageNumber;

            Label {
                anchors {
                    left: parent.left;
                    leftMargin: theme.paddingLarge * model.level;
                    right: pageNumberLbl.left;
                    rightMargin: theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                elide: Text.ElideRight;
                text: model.title;
            }
            Label {
                id: pageNumberLbl
                anchors {
                    right: parent.right;
                    rightMargin: theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                text: model.pageNumber;
            }

            onClicked: {
                page.canvas.currentPageNumber = model.pageNumber;
                pageStack.navigateBack(PageStackAction.Animated);
            }
        }
    }
}
