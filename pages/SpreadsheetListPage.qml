import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1 as Calligra

Page {
    id: page;

    property string title;
    property Item canvas;

    SilicaListView {
        anchors.fill: parent;

        header: PageHeader { title: page.title; }

        model: Calligra.SpreadsheetListModel {
            canvas: page.canvas;
        }

        delegate: BackgroundItem {
            highlighted: model.index == page.canvas.currentSheet;

            Label {
                anchors {
                    left: parent.left;
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
