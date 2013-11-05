import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Office.Files 1.0
import Sailfish.Office.Dropbox 1.0
//import QtMobility.gallery 1.1

Page {

    allowedOrientations: Orientation.All;

    Component {
        id: delegate
        BackgroundItem {
            id: delegateItem
            width: view.width
            height: thumbnail.height
            enabled: model.count > 0 || model.count === -1;
            opacity: enabled ? 1.0 : 0.5

            Label {
                id: countLabel
                objectName: "countLabel"
                anchors {
                    right: thumbnail.left
                    rightMargin: Theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
                text: model.count > -1 ? model.count : ""
                color: delegateItem.down ? Theme.highlightColor : Theme.primaryColor
                font.pixelSize: Theme.fontSizeLarge
            }

            // Load icon from a plugin
            HighlightImage {
                id: thumbnail
                x: width - Theme.paddingLarge
                width: Theme.itemSizeExtraLarge
                height: width
                source: model.icon;
                opacity: delegateItem.down ? 0.5 : 1
                highlighted: delegateItem.highlighted;
            }

            Label {
                id: titleLabel
                objectName: "titleLabel"
                elide: Text.ElideRight
                font.pixelSize: Theme.fontSizeLarge
                text: model.title
                color: delegateItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                anchors {
                    left: thumbnail.right
                    right: parent.right
                    leftMargin: Theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
            }

            onClicked: {
                if(model.needsSetup)
                {
                    pageStack.push(Qt.resolvedUrl(model.setupPageUrl));
                }
                else
                {
                    //console.debug("Model name: " + providerModel.objectName);
                    window.pageStack.push(model.page != "" ? Qt.resolvedUrl(model.page) : fileListPage, {
                        title: model.title,
                        model: model.providerModel,
                        //thumbnailDelegate: thumbnail != "" ? thumbnail : Qt.resolvedUrl("GridImageThumbnail.qml")
                        } )
                }
            }
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
            DropboxDocumentProvider {
                id: dropboxProvider
                needAuthenticate: controllerMIT.needAuthenticate
                fileListModel: folderListModel;
            }
        }
        PullDownMenu {
            MenuItem {
                //: Menu item for starting DropBox setup
                //% "Set up DropBox..."
                text: qsTrId("sailfish-office-me-setup_dropbox");
                onClicked: pageStack.push(dropboxProvider.setupPageUrl);
            }
        }
    }

    Component.onCompleted: {
        controllerMIT.need_authenticate();
        controllerMIT.getlistoffolder()
    }

    Component {
        id: fileListPage;
        FileListPage { }
    }
}
