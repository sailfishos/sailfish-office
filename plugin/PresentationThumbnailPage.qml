import QtQuick 2.0
import Sailfish.Silica 1.0
import org.kde.calligra 1.0 as Calligra

Page {
    id: page;

    QtObject {
        id: theme;
        property int paddingMedium: 12;
        property int paddingSmall: 8;
    }

    property QtObject document;

    allowedOrientations: Orientation.All;

    SilicaGridView {
        id: grid;

        anchors.fill: parent;

        cellWidth: page.width / 3;
        cellHeight: cellWidth * 0.75;

        currentIndex: page.document.currentIndex;

        //: Page with slide overview
        //% "Slides"
        header: PageHeader { title: qsTrId( "sailfish-office-he-slide_index" ) }

        model: Calligra.ContentsModel {
            document: page.document;
            thumbnailSize.width: grid.cellWidth;
            thumbnailSize.height: grid.cellHeight;
        }

        delegate: Item {
            id: base;
            width: GridView.view.cellWidth;
            height: GridView.view.cellHeight;

            Rectangle {
                anchors.fill: parent;
                border.width: 1;

                Calligra.ImageDataItem {
                    anchors.fill: parent;
                    data: model.thumbnail;
                }

                Rectangle {
                    anchors.centerIn: parent;
                    width: label.width + theme.paddingMedium;
                    height: label.height;
                    radius: theme.paddingSmall;
                    color: base.GridView.isCurrentItem ? theme.highlightColor : theme.secondaryHighlightColor;
                }

                Label {
                    id: label;
                    anchors.centerIn: parent;
                    text: model.contentIndex + 1;
                }
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    page.document.currentIndex = model.contentIndex;
                    pageStack.navigateBack(PageStackAction.Animated);
                }
            }
        }
    }
}
