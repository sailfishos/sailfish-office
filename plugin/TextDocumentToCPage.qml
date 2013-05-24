import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

Page {
    id: page;

    property Item canvas;

    allowedOrientations: Orientation.All;

    SilicaListView {
        anchors.fill: parent;

        //: Page with Text document index
        //% "Index"
        header: PageHeader { title: qsTrId( "sailfish-office-he-index") }

        model: Calligra.TextToCModel {
            canvas: page.canvas;
        }

        delegate: BackgroundItem {
            highlighted: model.page == page.canvas.currentPageNumber;

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
