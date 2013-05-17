import QtQuick 1.1
import Sailfish.Silica 1.0
import Sailfish.Office.Files 1.0
import QtMobility.gallery 1.1

Page {

    allowedOrientations: Orientation.All;

    Component {
        id: delegate
        BackgroundItem {
            id: delegateItem
            width: view.width
            height: thumbnail.height
            enabled: model.count > 0
            opacity: enabled ? 1.0 : 0.5

            Label {
                id: countLabel
                objectName: "countLabel"
                anchors {
                    right: thumbnail.left
                    rightMargin: theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
                text: model.count
                color: delegateItem.down ? theme.highlightColor : theme.primaryColor
                font.pixelSize: theme.fontSizeLarge
            }

            // Load icon from a plugin
            Image {
                id: thumbnail
                x: width - theme.paddingLarge
                width: theme.itemSizeExtraLarge
                height: width
                source: model.icon;
                opacity: delegateItem.down ? 0.5 : 1
            }

            Label {
                id: titleLabel
                objectName: "titleLabel"
                elide: Text.ElideRight
                font.pixelSize: theme.fontSizeLarge
                text: model.title
                color: delegateItem.down ? theme.highlightColor : theme.primaryColor
                anchors {
                    left: thumbnail.right
                    right: parent.right
                    leftMargin: theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
            }

            onClicked: {
                    console.debug("Model name: " + providerModel.objectName);
                    window.pageStack.push(model.page != "" ? Qt.resolvedUrl(model.page) : fileListPage, {
                    title: model.title,
                    model: model.providerModel,
                    //thumbnailDelegate: thumbnail != "" ? thumbnail : Qt.resolvedUrl("GridImageThumbnail.qml")
            } ) }
        }
    }

    SilicaListView {
        id: view
        objectName: "docListView"

        anchors.fill: parent
        delegate: delegate
        header: PageHeader {
            //: Application title
            //% "Documents"
            title: qsTrId("sailfish-office-he-apptitle");
        }
        model: DocumentProviderListModel {
            id: documentSources
            TrackerDocumentProvider {
            }
        }
    }

    Component {
        id: fileListPage;
        FileListPage { }
    }
}
