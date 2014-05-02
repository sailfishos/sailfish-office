import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    CoverPlaceholder {
        //: Cover placeholder shown when there are no documents
        //% "No documents"
        text: qsTrId("sailfish-office-la-cover_no_documents")
        icon.source: "image://theme/icon-launcher-office"
        visible: window.documentItem === null && fileListView.count == 0
    }
    ListView {
        id: fileListView

        property real itemHeight: Theme.iconSizeSmall + Theme.paddingSmall*2

        clip: true
        interactive: false
        model: window.fileListModel
        visible: window.documentItem === null
        y: Theme.paddingLarge
        width: parent.width
        height: 7*itemHeight

        delegate: Item {
            width: fileListView.width
            height: fileListView.itemHeight
            Image {
                id: icon

                property string fileMimeType: window.mimeToIcon(model.fileMimeType)
                anchors {
                    left: parent.left
                    leftMargin: Theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
                source: fileMimeType
                sourceSize { width: Theme.iconSizeSmall; height: Theme.iconSizeSmall }
                states: State {
                    when: icon.fileMimeType === ""
                    PropertyChanges {
                        target: icon
                        source: "image://theme/icon-l-document"
                    }
                }
            }
            Label {
                anchors {
                    left: icon.right
                    leftMargin: Theme.paddingMedium
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    rightMargin: Theme.paddingLarge
                }
                text: model.fileName
                truncationMode: TruncationMode.Fade
            }
        }
    }

    Item {
        property bool isPortrait: !pageStack.currentPage || pageStack.currentPage.isPortrait
        anchors.centerIn: parent
        width: isPortrait ? parent.width : parent.height
        height: isPortrait ? parent.height : parent.width
        rotation: isPortrait ? 0 : 90

        Image {
            id: previewImage
            anchors.fill: parent
            property QtObject coverWindow;
            property bool isGrabAvailable: typeof previewImage.grabToImage !== 'undefined';
            function updatePreview() {
                if (!isGrabAvailable)
                    return;
                if (window.documentItem && applicationWindow.visible) {
                    window.documentItem.grabToImage(function(result) { previewImage.source = result.url; },
                                                             Qt.size(width, height));
                }
            }
            Component.onCompleted: {
                if (isGrabAvailable)
                    coverWindow = coverWindowAccessor.coverWindow();
            }
            Connections {
                id: windowConnections;
                target: previewImage.coverWindow;
                onVisibilityChanged: previewImage.updatePreview();
                ignoreUnknownSignals: true;
            }
            Connections {
                target: window
                onDocumentItemChanged: { previewImage.updatePreview(); }
                onOrientationChanged: { previewImage.updatePreview(); }
            }
        }

        // This bit can be removed once we're fully on a Qt 5.2 stack.
        ShaderEffectSource {
            anchors.fill: parent
            visible: !previewImage.coverWindow
            sourceItem: visible ? window.documentItem : null
            textureSize: Qt.size(width, height)
            live: status === Cover.Active
        }
    }
}

