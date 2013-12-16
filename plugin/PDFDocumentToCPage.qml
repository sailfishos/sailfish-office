import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page;
    signal pageSelected(int pageNumber);

    property alias tocModel: tocListView.model;

    allowedOrientations: Orientation.All;

    SilicaListView {
        id: tocListView
        anchors.fill: parent;

        //: Page with PDF index
        //% "Index"
        header: PageHeader { title: qsTrId( "sailfish-office-he-pdf_index" ); }

        delegate: BackgroundItem {
            id: bg;

            Label {
                anchors {
                    left: parent.left;
                    leftMargin: Theme.paddingLarge * (model.level+1);
                    right: pageNumberLbl.left;
                    rightMargin: Theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                elide: Text.ElideRight;
                text: (model.title === undefined) ? "" : model.title;
                color: bg.highlighted ? Theme.highlightColor : Theme.primaryColor;
                truncationMode: TruncationMode.Fade;
            }
            Label {
                id: pageNumberLbl
                anchors {
                    right: parent.right;
                    rightMargin: Theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                text: (model.pageNumber === undefined) ? "" : model.pageNumber;
                color: bg.highlighted ? Theme.highlightColor : Theme.primaryColor;
            }

            onClicked: {
                page.pageSelected(model.pageNumber - 1);
                pageStack.navigateBack(PageStackAction.Animated);
            }
        }
    }
}
