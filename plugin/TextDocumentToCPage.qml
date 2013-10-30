import QtQuick 2.0
import Sailfish.Silica 1.0
import org.kde.calligra 1.0 as Calligra

Page {
    id: page;

    property QtObject document;

    allowedOrientations: Orientation.All;

    SilicaListView {
        anchors.fill: parent;

        //: Page with Text document index
        //% "Index"
        header: PageHeader { title: qsTrId( "sailfish-office-he-index") }

        model: Calligra.ContentsModel {
            document: page.document;
        }

        delegate: BackgroundItem {
            highlighted: model.contentIndex == page.document.currentIndex;

            Label {
                anchors {
                    left: parent.left;
                    leftMargin: Theme.paddingLarge * model.level;
                    right: pageNumberLbl.left;
                    rightMargin: Theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                elide: Text.ElideRight;
                text: model.title;
            }
            Label {
                id: pageNumberLbl
                anchors {
                    right: parent.right;
                    rightMargin: Theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                text: model.contentIndex;
            }

            onClicked: {
                page.document.currentIndex = model.contentIndex;
                pageStack.navigateBack(PageStackAction.Animated);
            }
        }
    }
}
