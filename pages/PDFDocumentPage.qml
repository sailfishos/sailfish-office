import QtQuick 1.1
import Sailfish.Silica 1.0
import com.jolla.components.views 1.0
import "../components"
import org.calligra.CalligraComponents 0.1 as Calligra
import Sailfish.Office.PDF 1.0 as PDF

SplitViewPage {
    id: base;

    property string title;
    property string path;
    property string mimeType;

    allowedOrientations: window.allowedOrientations

    DocumentsSharingList {
        visualParent: page;
        path: base.path;
        mimeType: base.mimeType;
    }

    contentItem: Item {
        clip: true;

        SilicaListView {
            anchors.fill: parent;

            spacing: theme.paddingLarge;

            model: PDF.PageModel { pageWidth: base.width; }

            delegate: Rectangle {
                width: model.width;
                height: model.height;

                Label { anchors.centerIn: parent; text: "Loading"; color: "black" }

                PDF.Page {
                    id: pageImage;
                    anchors.fill: parent;
                    content: model.page;
                    Component.onDestruction: parent.ListView.view.model.discard( model.index );
                }
            }

            children: ScrollDecorator { }

            MouseArea { anchors.fill: parent; onClicked: base.toggleSplit(); }
        }
    }

    PDF.Document {
        id: pdfDocument;
        source: base.path;
    }
}
