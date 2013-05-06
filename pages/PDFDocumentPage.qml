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
        visualParent: base;
        title: base.title;
        path: base.path;
        mimeType: base.mimeType;
    }

    contentItem: Item {
        clip: true;

        SilicaFlickable {
            id: view;

            anchors.fill: parent;

            contentWidth: content.width;
            contentHeight: content.height;

            transform: Scale { id: viewScale; }

            Column {
                id: content;
                spacing: theme.paddingLarge;

                Repeater {
                    id: repeater;
                    model: PDF.PageModel { id: pdfModel; pageWidth: base.width; }

                    delegate: Rectangle {
                        width: model.width;
                        height: model.height;

                        PDF.Page {
                            id: pageImage;
                            anchors.fill: parent;
                            content: model.page;
                        }

                        function updateVisibility() {
                            var fPos = mapToItem( view, x, y );
                            if( fPos.y + height < 0 || fPos.y > view.height )
                            {
                                pdfModel.discard( model.index );
                            }
                        }
                    }

                    function updateVisibility() { }
                }
            }

            children: ScrollDecorator { }

            PinchArea {
                anchors.fill: parent;

                onPinchUpdated: {
                    var viewCenter = mapToItem( view, pinch.center.x, pinch.center.y );
                    viewScale.origin.x = viewCenter.x;
                    viewScale.origin.y = viewCenter.y;
                    viewScale.xScale = pinch.scale;
                    viewScale.yScale = pinch.scale;
                }

                onPinchFinished: {
                    pdfModel.pageWidth = pdfModel.pageWidth * viewScale.xScale;
                    viewScale.xScale = 1;
                    viewScale.yScale = 1;
                    viewScale.origin.x = 0;
                    viewScale.origin.y = 0;
                }

                MouseArea { anchors.fill: parent; onClicked: base.toggleSplit(); }
            }

            onContentYChanged: {
                for( var i = 0; i < content.children.length; ++i ) {
                    content.children[i].updateVisibility();
                }
            }
        }
    }

    PDF.Document {
        id: pdfDocument;
        source: base.path;
    }

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if(status == PageStatus.Active) {
            if(pageStack.nextPage(base) === null) {
                pageStack.pushAttached( tocPage );
            }
        }
    }
    Component {
        id: tocPage;
        PDFDocumentToCPage {
            title: base.title;
            tocModel: pdfDocument.tocModel
            onPageSelected: view.contentY = repeater.itemAt(pageNumber - 1).y;
        }
    }
}
