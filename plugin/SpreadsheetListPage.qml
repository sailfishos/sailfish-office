import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

Page {
    id: page;

    property string title;
    property Item canvas;

    allowedOrientations: Orientation.All;

    SilicaListView {
        anchors.fill: parent;

        header: PageHeader { title: page.title; }

        model: Calligra.SpreadsheetListModel {
            canvas: page.canvas;
            thumbnailSize.width: theme.itemSizeLarge;
            thumbnailSize.height: theme.itemSizeLarge;
        }

        delegate: BackgroundItem {
            highlighted: model.index == page.canvas.currentSheet;

            Calligra.Thumbnail {
                id: thumbnail;

                anchors {
                    left: parent.left;
                    verticalCenter: parent.verticalCenter;
                }

                width: parent.height;
                height: parent.height;

                content: model.thumbnail;
            }

            Label {
                anchors {
                    left: thumbnail.right;
                    leftMargin: theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }

                text: model.sheetName;
            }

            onClicked: {
                page.canvas.currentSheet = model.index;
                pageStack.navigateBack(PageStackAction.Animated);
            }
        }
    }
}
