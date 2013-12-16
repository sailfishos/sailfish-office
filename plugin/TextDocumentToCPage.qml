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
            id: bg;
            Label {
                anchors {
                    left: parent.left;
                    leftMargin: Theme.paddingLarge * (model.level+1);
                    right: pageNumberLbl.left;
                    rightMargin: Theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                text: model.title;
                color: (model.contentIndex == page.document.currentIndex || bg.highlighted) ? Theme.highlightColor : Theme.primaryColor;
                truncationMode: TruncationMode.Fade
            }
            Label {
                id: pageNumberLbl
                anchors {
                    right: parent.right;
                    rightMargin: Theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                text: model.contentIndex + 1;
                color: (model.contentIndex == page.document.currentIndex || bg.highlighted) ? Theme.highlightColor : Theme.primaryColor;
            }

            onClicked: {
                page.document.currentIndex = model.contentIndex;
                pageStack.navigateBack(PageStackAction.Animated);
            }
        }
    }
}
