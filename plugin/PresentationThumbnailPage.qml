import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

Page {
    id: page;

    property string title;
    property Item canvas;
    property Item view;

    allowedOrientations: Orientation.All;

    SilicaGridView {
        id: view;

        anchors.fill: parent;

        cellWidth: page.width / 3;
        cellHeight: cellWidth * 0.75;

        currentIndex: page.view.currentIndex;

        //: Page with slide overview
        //% "Slides"
        header: PageHeader { title: qsTrId( "sailfish-office-he-slide_index" ) }

        model: Calligra.PresentationModel {
            canvas: page.canvas;
            thumbnailSize.width: view.cellWidth;
            thumbnailSize.height: view.cellHeight;
        }

        delegate: Item {
            id: base;
            width: GridView.view.cellWidth;
            height: GridView.view.cellHeight;

            Rectangle {
                anchors.fill: parent;
                border.width: 1;

                Calligra.Thumbnail {
                    anchors.fill: parent;
                    content: model.thumbnail;
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
                    text: model.index + 1;
                }
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    page.view.currentIndex = model.index;
                    pageStack.navigateBack(PageStackAction.Animated);
                }
            }
        }
    }
}
