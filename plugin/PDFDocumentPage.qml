/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Contact: Robin Burchell <robin.burchell@jolla.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Office.PDF 1.0 as PDF
import org.nemomobile.configuration 1.0
import org.nemomobile.notifications 1.0
import QtQuick.LocalStorage 2.0
import "PDFStorage.js" as PDFStorage

DocumentPage {
    id: base

    property var _settings // Handle save and restore the view settings using PDFStorage

    busy: (!pdfDocument.loaded && !pdfDocument.failure) || pdfDocument.searching
    source: pdfDocument.source
    indexCount: pdfDocument.pageCount
    drawerVisible: !(pdfDocument.failure || pdfDocument.locked)
    documentItem: view

    function savePageSettings() {
        if (!rememberPositionConfig.value || pdfDocument.failure || pdfDocument.locked) {
            return
        }
        
        if (!_settings) {
            _settings = new PDFStorage.Settings(pdfDocument.source)
        }
        var last = view.getPagePosition()
        _settings.setLastPage(last[0] + 1, last[1], last[2], view.itemWidth)
    }

    attachedPage: Component {
        PDFDocumentToCPage {
            tocModel: pdfDocument.tocModel
            pageCount: pdfDocument.pageCount
            onPageSelected: view.goToPage(pageNumber)
        }
    }

    // Save and restore view settings when needed.
    onStatusChanged: if (status == PageStatus.Inactive) { savePageSettings() }

    Connections {
        target: Qt.application
        onAboutToQuit: savePageSettings()
    }
    Connections {
        target: view
        onPageSizesReady: {
            if (rememberPositionConfig.value) {
                if (!_settings) {
                    _settings = new PDFStorage.Settings(pdfDocument.source)
                }
                var last = _settings.getLastPage()
                if (last[3] > 0) {
                    view.itemWidth = last[3]
                    view.adjust()
                }
                view.goToPage( last[0] - 1, last[1], last[2] )
            }
        }
    }

    Binding {
        target: base
        property: "forwardNavigation"
        value: false
        when: (pdfDocument.failure || pdfDocument.locked)
    }

    Loader {
        parent: base
        sourceComponent: (pdfDocument.failure || pdfDocument.locked) ? placeholderComponent : null
        anchors.verticalCenter: parent.verticalCenter
    }

    PDFView {
        id: view

        // for cover state
        property bool contentAvailable: pdfDocument.loaded && !(pdfDocument.failure || pdfDocument.locked)
        property alias title: base.title
        property alias mimeType: base.mimeType

        anchors.fill: parent
        anchors.bottomMargin: toolbar.offset
        document: pdfDocument
        onClicked: base.open = !base.open
        onLinkClicked: {
            base.open = false
            contextMenuLinks.url = linkTarget
            hook.showMenu(contextMenuLinks)
        }
        onAnnotationClicked: {
            base.open = false
            switch (annotation.type) {
            case PDF.Annotation.Highlight:
                contextMenuHighlight.annotation = annotation
                hook.showMenu(contextMenuHighlight)
                break
            case PDF.Annotation.Caret:
            case PDF.Annotation.Text:
                pdfDocument.edit(annotation)
                break
            default:
            }
        }
        onAnnotationLongPress: {
            base.open = false
            switch (annotation.type) {
            case PDF.Annotation.Highlight:
                contextMenuHighlight.annotation = annotation
                hook.showMenu(contextMenuHighlight)
                break
            case PDF.Annotation.Caret:
            case PDF.Annotation.Text:
                contextMenuText.annotation = annotation
                hook.showMenu(contextMenuText)
                break
            default:
            }
        }
        onLongPress: {
            base.open = false
            contextMenuText.at = pressAt
            contextMenuText.annotation = null
            hook.showMenu(contextMenuText)
        }
        clip: anchors.bottomMargin > 0 || base.status !== PageStatus.Active
    }

    ToolBar {
        id: toolbar

        width: parent.width
        height: base.orientation == Orientation.Portrait
                || base.orientation == Orientation.InvertedPortrait
                ? Theme.itemSizeLarge
                : Theme.itemSizeSmall
        anchors.top: view.bottom
        flickable: view
        forceHidden: base.open || pdfDocument.failure || pdfDocument.locked
                     || contextMenuLinks.active
                     || contextMenuHighlight.active
                     || contextMenuText.active
        autoShowHide: !row.active

        Connections {
            target: view.selection
            onSelectedChanged: if (view.selection.selected) toolbar.show()
        }

        Notification {
            id: notice
            property bool published
            function show(info) {
                previewSummary = info
                if (published) close()
                publish()
                published = true
            }
            function hide() {
                if (published) close()
                published = false
            }
        }

        // Toolbar contain.
        Row {
            id: row
            property bool active: pageCount.highlighted
                                  || search.highlighted
                                  || !search.iconized
                                  || textButton.highlighted
                                  || highlightButton.highlighted
                                  || view.selection.selected
            property Item activeItem
            property real itemWidth: toolbar.width / children.length
            height: parent.height

            function toggle(item) {
                notice.hide()
                view.selection.unselect()
                if (row.activeItem === item) {
                    row.activeItem = null
                } else {
                    row.activeItem = item
                }
            }

            SearchBarItem {
                id: search
                width: toolbar.width
                iconizedWidth: row.itemWidth
                height: parent.height

                modelCount: pdfDocument.searchModel ? pdfDocument.searchModel.count : -1

                onRequestSearch: pdfDocument.search(text, view.currentPage - 1)
                onRequestPreviousMatch: view.prevSearchMatch()
                onRequestNextMatch: view.nextSearchMatch()
                onRequestCancel: pdfDocument.cancelSearch()
                onClicked: row.toggle(search)
            }
            BackgroundItem {
                id: textTool
                property bool first: true

                width: row.itemWidth
                height: parent.height
                highlighted: pressed || textButton.pressed
                onClicked: {
                    row.toggle(textTool)
                    if (textTool.first) {
                        //% "Tap where you want to add a note"
                        notice.show(qsTrId("sailfish-office-la-notice-anno-text"))
                        textTool.first = false
                    }
                }
                IconButton {
                    id: textButton
                    anchors.centerIn: parent
                    highlighted: pressed || textTool.pressed || row.activeItem === textTool
                    icon.source: "image://theme/icon-m-notifications"
                    onClicked: textTool.clicked(mouse)
                }
                MouseArea {
                    parent: row.activeItem === textTool ? view : null
                    anchors.fill: parent
                    onClicked: {
                        var annotation = textComponent.createObject(contextMenuText)
                        var pt = Qt.point(view.contentX + mouse.x,
                                          view.contentY + mouse.y)
                        pdfDocument.create(annotation,
                                           function() {
                                               var at = view.getPositionAt(pt)
                                               annotation.attachAt(pdfDocument,
                                                                   at[0], at[2], at[1])
                                           })
                        row.toggle(textTool)
                    }
                }
            }
            BackgroundItem {
                id: highlightTool
                property bool first: true

                function highlightSelection() {
                    var anno = highlightComponent.createObject(highlightTool)
                    anno.color = highlightColorConfig.value
                    anno.style = highlightStyleConfig.toEnum(highlightStyleConfig.value)
                    anno.attach(pdfDocument, view.selection)
                    toolbar.hide()
                }

                width: row.itemWidth
                height: parent.height
                highlighted: pressed || highlightButton.pressed
                onClicked: {
                    if (view.selection.selected) {
                        highlightSelection()
                        view.selection.unselect()
                        return
                    }
                    row.toggle(highlightTool)
                    if (highlightTool.first) {
                        //% "Tap and move your finger over the area"
                        notice.show(qsTrId("sailfish-office-la-notice-anno-highlight"))
                        highlightTool.first = false
                    }
                }

                Component {
                    id: highlightComponent
                    PDF.HighlightAnnotation { }
                }

                IconButton {
                    id: highlightButton
                    anchors.centerIn: parent
                    highlighted: pressed || highlightTool.pressed || row.activeItem === highlightTool
                    icon.source: "image://theme/icon-m-edit"
                    onClicked: highlightTool.clicked(mouse)
                }
                MouseArea {
                    parent: row.activeItem === highlightTool ? view : null
                    anchors.fill: parent
                    preventStealing: true
                    onPressed: {
                        view.selection.selectAt(Qt.point(view.contentX + mouse.x,
                                                         view.contentY + mouse.y))
                    }
                    onPositionChanged: {
                        if (view.selection.count < 1) {
                            view.selection.selectAt(Qt.point(view.contentX + mouse.x,
                                                             view.contentY + mouse.y))
                        } else {
                            view.selection.handle2 = Qt.point(view.contentX + mouse.x,
                                                              view.contentY + mouse.y)
                        }
                    }
                    onReleased: {
                        if (view.selection.selected) highlightTool.highlightSelection()
                        row.toggle(highlightTool)
                    }
                    Binding {
                        target: view
                        property: "selectionDraggable"
                        value: row.activeItem !== highlightTool
                    }
                }
            }
            BackgroundItem {
                id: pageCount
                width: row.itemWidth
                height: parent.height
                Label {
                    id: pageLabel
                    anchors.centerIn: parent
                    color: pageCount.highlighted ? Theme.highlightColor : Theme.primaryColor
                    text: view.currentPage + " | " + view.document.pageCount
                }
                onClicked: {
                    row.toggle(pageCount)
                    base.pushAttachedPage()
                }
            }
        }
    }

    ContextMenu {
        id: contextMenuLinks
        property alias url: linkTarget.text

        InfoLabel {
            id: linkTarget
            font.pixelSize: Theme.fontSizeSmall
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            maximumLineCount: 4
            color: Theme.highlightColor
            opacity: .6
        }
        MenuItem {
            text: (contextMenuLinks.url.indexOf("http:") === 0
                   || contextMenuLinks.url.indexOf("https:") === 0)
                  //% "Open in browser"
                  ? qsTrId("sailfish-office-me-pdf-open-browser")
                  //% "Open in external application"
                  : qsTrId("sailfish-office-me-pdf-open-external")
            onClicked: Qt.openUrlExternally(contextMenuLinks.url)
        }
        MenuItem {
            //% "Copy to clipboard"
            text: qsTrId("sailfish-office-me-pdf-copy-link")
            onClicked: Clipboard.text = contextMenuLinks.url
        }
    }

    ContextMenu {
        id: contextMenuText
        property variant annotation
        property point at

        MenuItem {
            visible: contextMenuText.annotation === undefined
                     || contextMenuText.annotation === null
            //% "Add note"
            text: qsTrId("sailfish-office-me-pdf-txt-anno-add")
            onClicked: {
                var annotation = textComponent.createObject(contextMenuText)
                annotation.color = "#202020"
                pdfDocument.create(annotation,
                                   function() {
                                       var at = view.getPositionAt(contextMenuText.at)
                                       annotation.attachAt(pdfDocument, at[0], at[2], at[1])
                                   })
            }
        }
        MenuItem {
            visible: contextMenuText.annotation !== undefined
                     && contextMenuText.annotation !== null
            //% "Edit"
            text: qsTrId("sailfish-office-me-pdf-txt-anno-edit")
            onClicked: pdfDocument.edit(contextMenuText.annotation)
        }
        MenuItem {
            visible: contextMenuText.annotation !== undefined
                     && contextMenuText.annotation !== null
            //% "Delete"
            text: qsTrId("sailfish-office-me-pdf-txt-anno-clear")
            onClicked: contextMenuText.annotation.remove()
        }
        Component {
            id: textComponent
            PDF.TextAnnotation { }
        }
    }

    ContextMenu {
        id: contextMenuHighlight
        property variant annotation

        InfoLabel {
            id: infoContents
            visible: infoContents.text != ""
            width: parent.width
            anchors.topMargin: Theme.paddingSmall
            height: implicitHeight + 2 * Theme.paddingSmall
            font.pixelSize: Theme.fontSizeSmall
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            maximumLineCount: 2
            color: Theme.highlightColor
            opacity: .6
            text: {
                if (contextMenuHighlight.annotation !== null
                    && contextMenuHighlight.annotation !== undefined
                    && contextMenuHighlight.annotation.contents != "") {
                    return (contextMenuHighlight.annotation.author != ""
                            ? "(" + contextMenuHighlight.annotation.author + ") " : "")
                    + contextMenuHighlight.annotation.contents
                } else {
                    return ""
                }
            }
        }
        Row {
            height: Theme.itemSizeExtraSmall
            Repeater {
                id: colors
                model: ["#db431c", "#ffff00", "#8afa72", "#00ffff",
                        "#3828f9", "#a328c7", "#ffffff", "#989898",
                        "#000000"]
                delegate: Rectangle {
                    width: contextMenuHighlight.width / colors.model.length
                    height: parent.height
                    color: modelData
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            contextMenuHighlight.hide()
                            contextMenuHighlight.annotation.color = color
                            highlightColorConfig.value = modelData
                        }
                    }
                }
            }
        }
        Row {
            height: Theme.itemSizeExtraSmall
            Repeater {
                id: styles
                model: [{"style": PDF.HighlightAnnotation.Highlight,
                         "label": "abc"},
                        {"style": PDF.HighlightAnnotation.Squiggly,
                         "label": "a̰b̰c̰"},
                        {"style": PDF.HighlightAnnotation.Underline,
                         "label": "<span style=\"text-decoration:underline\">abc</span>"},
                        {"style": PDF.HighlightAnnotation.StrikeOut,
                         "label": "<span style=\"text-decoration:line-through\">abc</span>"}]
                delegate: BackgroundItem {
                    id: bgStyle
                    width: contextMenuHighlight.width / styles.model.length
                    height: parent.height
                    onClicked: {
                        contextMenuHighlight.hide()
                        contextMenuHighlight.annotation.style = modelData["style"]
                        highlightStyleConfig.value = highlightStyleConfig.fromEnum(modelData["style"])
                    }
                    Label {
                        anchors.centerIn: parent
                        text: modelData["label"]
                        textFormat: Text.RichText
                        color: bgStyle.highlighted
                               || (contextMenuHighlight.annotation !== undefined
                                   && contextMenuHighlight.aannotation !== null
                                   && contextMenuHighlight.annotation.style == modelData["style"])
                               ? Theme.highlightColor : Theme.primaryColor
                        Rectangle {
                            visible: modelData["style"] == PDF.HighlightAnnotation.Highlight
                            anchors.fill: parent
                            color: bgStyle.highlighted ? Theme.highlightColor : Theme.primaryColor
                            opacity: 0.4
                            z: -1
                        }
                    }
                }
            }
        }
        MenuItem {
            visible: contextMenuHighlight.annotation !== undefined
                     && contextMenuHighlight.annotation !== null
            text: contextMenuHighlight.annotation !== undefined
                  && contextMenuHighlight.annotation !== null
                  && contextMenuHighlight.annotation.contents == ""
                  //% "Add a comment"
                  ? qsTrId("sailfish-office-me-pdf-hl-anno-comment")
                  //% "Edit the comment"
                  : qsTrId("sailfish-office-me-pdf-hl-anno-comment-edit")
            onClicked: {
                if (contextMenuHighlight.annotation.contents == "") {
                    pdfDocument.create(contextMenuHighlight.annotation)
                } else {
                    pdfDocument.edit(contextMenuHighlight.annotation)
                }
            }
        }
        MenuItem {
            //% "Clear"
            text: qsTrId("sailfish-office-me-pdf-hl-anno-clear")
            onClicked: contextMenuHighlight.annotation.remove()
        }
    }

    PDF.Document {
        id: pdfDocument
        source: base.path
        autoSavePath: base.path

        function create(annotation, callback) {
            var isText = (annotation.type == PDF.Annotation.Text
                          || annotation.type == PDF.Annotation.Caret)
            var dialog = pageStack.push(Qt.resolvedUrl("PDFAnnotationNew.qml"),
                                        {"isTextAnnotation": isText})
            dialog.accepted.connect(function() {
                annotation.contents = dialog.text
            })
            if (callback !== undefined) dialog.accepted.connect(callback)
        }
        function edit(annotation) {
            var edit = pageStack.push(Qt.resolvedUrl("PDFAnnotationEdit.qml"),
                                      {"annotation": annotation})
            edit.remove.connect(function() {
                pageStack.pop()
                annotation.remove()
            })
        }
    }

    Component {
        id: placeholderComponent

        Column {
            width: base.width

            InfoLabel {
                text: pdfDocument.failure ? //% "Broken file"
                                            qsTrId("sailfish-office-me-broken-pdf")
                                          : //% "Locked file"
                                            qsTrId("sailfish-office-me-locked-pdf")
            }

            InfoLabel {
                font.pixelSize: Theme.fontSizeLarge
                color: Theme.rgba(Theme.highlightColor, 0.4)
                text: pdfDocument.failure ? //% "Cannot read the PDF document"
                                            qsTrId("sailfish-office-me-broken-pdf-hint")
                                          : //% "Enter password to unlock"
                                            qsTrId("sailfish-office-me-locked-pdf-hint")
            }

            Item {
                visible:password.visible
                width: 1
                height: Theme.paddingLarge
            }

            PasswordField {
                id: password

                visible: pdfDocument.locked
                x: Theme.horizontalPageMargin
                width: parent.width - 2*x
                EnterKey.enabled: text
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: {
                    focus = false
                    pdfDocument.requestUnLock(text)
                    text = ""
                }

                Component.onCompleted: {
                    if (visible)
                        forceActiveFocus()
                }
            }
        }
    }

    ConfigurationValue {
        id: rememberPositionConfig
        
        key: "/apps/sailfish-office/settings/rememberPosition"
        defaultValue: true
    }
    ConfigurationValue {
       id: highlightColorConfig
       key: "/apps/sailfish-office/settings/highlightColor"
       defaultValue: "#ffff00"
    }
    ConfigurationValue {
       id: highlightStyleConfig
       key: "/apps/sailfish-office/settings/highlightStyle"
       defaultValue: "highlight"
       
       function toEnum(configVal) {
           if (configVal == "highlight") {
               return PDF.HighlightAnnotation.Highlight
           } else if (configVal == "squiggly") {
               return PDF.HighlightAnnotation.Squiggly
           } else if (configVal == "underline") {
               return PDF.HighlightAnnotation.Underline
           } else if (configVal == "strike") {
               return PDF.HighlightAnnotation.StrikeOut
           } else {
               return PDF.HighlightAnnotation.Highlight
           }
       }
       function fromEnum(enumVal) {
            switch (enumVal) {
            case PDF.HighlightAnnotation.Highlight:
                return "highlight"
            case PDF.HighlightAnnotation.Squiggly:
                return "squiggly"
            case PDF.HighlightAnnotation.Underline:
                return "underline"
            case PDF.HighlightAnnotation.StrikeOut:
                return "strike"
            default:
                return "highlight"
            }
       }
    }

    Timer {
        id: updateSourceSizeTimer
        interval: 5000
        onTriggered: linkArea.sourceSize = Qt.size(base.width, pdfCanvas.height)
    }
}
