import QtQuick 2.0
import Sailfish.Silica 1.0
import org.kde.calligra 1.0 as Calligra

Page {
    id: page;

    property QtObject document;

    allowedOrientations: Orientation.All;

    QtObject {
        id: theme;
        property int itemSizeLarge: 64;
    }

    SilicaListView {
        anchors.fill: parent;

        //: Page with sheet selector
        //% "Sheets"
        header: PageHeader { title: qsTrId( "sailfish-office-he-sheet_index" ) }

        model: Calligra.ContentsModel {
            document: page.document;
            thumbnailSize.width: theme.itemSizeLarge;
            thumbnailSize.height: theme.itemSizeLarge;
        }

        delegate: BackgroundItem {
            highlighted: model.contentIndex == page.document.currentIndex;

            Calligra.ImageDataItem {
                id: thumbnail;

                anchors {
                    left: parent.left;
                    verticalCenter: parent.verticalCenter;
                }

                width: parent.height;
                height: parent.height;

                data: model.thumbnail;
            }

            Label {
                anchors {
                    left: thumbnail.right;
                    leftMargin: theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }

                text: model.title;
            }

            onClicked: {
                page.document.currentIndex = model.contentIndex;
                pageStack.navigateBack(PageStackAction.Animated);
            }
        }
    }
}
