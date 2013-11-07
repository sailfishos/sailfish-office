import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Silica.theme 1.0

Item {
    property alias detail: detailLabel.text
    property alias value: valueLabel.text

    width: parent.width
    height: Theme.itemSizeLarge

    Column {
        x: Theme.paddingLarge
        Label {
            id: detailLabel
            height: Theme.fontSizeLarge
            font.family: Theme.fontFamilyHeading
        }
        Label { id: valueLabel }
    }
}
