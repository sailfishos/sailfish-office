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
    property ContextMenu contextMenuLinks
    property ContextMenu contextMenuText
    property ContextMenu contextMenuHighlight

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
        onCanMoveBackChanged: if (canMoveBack) toolbar.show()
        onClicked: base.open = !base.open
        onLinkClicked: {
            base.open = false
            if (!contextMenuLinks) {
                contextMenuLinks = contextMenuLinksComponent.createObject(base)
            }
            contextMenuLinks.url = linkTarget
            hook.showMenu(contextMenuLinks)
        }
        onAnnotationClicked: {
            base.open = false
            switch (annotation.type) {
            case PDF.Annotation.Highlight:
                if (!contextMenuHighlight) {
                    contextMenuHighlight = contextMenuHighlightComponent.createObject(base)
                }
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
                if (!contextMenuHighlight) {
                    contextMenuHighlight = contextMenuHighlightComponent.createObject(base)
                }
                contextMenuHighlight.annotation = annotation
                hook.showMenu(contextMenuHighlight)
                break
            case PDF.Annotation.Caret:
            case PDF.Annotation.Text:
                if (!contextMenuText) {
                    contextMenuText = contextMenuTextComponent.createObject(base)
                }
                contextMenuText.annotation = annotation
                hook.showMenu(contextMenuText)
                break
            default:
            }
        }
        onLongPress: {
            base.open = false
            if (!contextMenuText) {
                contextMenuText = contextMenuTextComponent.createObject(base)
            }
            contextMenuText.at = pressAt
            contextMenuText.annotation = null
            hook.showMenu(contextMenuText)
        }
        clip: anchors.bottomMargin > 0 || base.status !== PageStatus.Active
    }

    ToolBar {
        id: toolbar

        property Notification notice

        width: parent.width
        height: base.orientation == Orientation.Portrait
                || base.orientation == Orientation.InvertedPortrait
                ? Theme.itemSizeLarge
                : Theme.itemSizeSmall
        anchors.top: view.bottom
        flickable: view
        forceHidden: base.open || pdfDocument.failure || pdfDocument.locked
                     || (contextMenuLinks && contextMenuLinks.active)
                     || (contextMenuHighlight && contextMenuHighlight.active)
                     || (contextMenuText && contextMenuText.active)
        autoShowHide: !row.active

        function noticeShow(message) {
            if (!notice) {
                notice = noticeComponent.createObject(toolbar)
            }
            notice.show(message)
        }

        Connections {
            target: view.selection
            onSelectedChanged: if (view.selection.selected) toolbar.show()
        }

        Component {
            id: noticeComponent
            Notification {
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
        }

        // Toolbar contain.
        Row {
            id: row
            property bool active: pageCount.highlighted
                                  || linkBack.visible
                                  || rotateButton.highlighted
                                  || search.highlighted
                                  || !search.iconized
                                  || textButton.highlighted
                                  || highlightButton.highlighted
                                  || view.selection.selected
            property Item activeItem
            property int nVisibleChildren: children.length - (linkBack.visible ? 0 : 1)
            property real itemWidth: Math.max(toolbar.width - pageCount.width, 0)
                                     / (nVisibleChildren - 1)
            height: parent.height

            function toggle(item) {
                if (toolbar.notice) toolbar.notice.hide()
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

                searching: pdfDocument.searching
                searchProgress: pdfDocument.searchModel ? pdfDocument.searchModel.fraction : 0.
                matchCount: pdfDocument.searchModel ? pdfDocument.searchModel.count : -1

                onRequestSearch: pdfDocument.search(text, view.currentPage - 1)
                onRequestPreviousMatch: view.prevSearchMatch()
                onRequestNextMatch: view.nextSearchMatch()
                onRequestCancel: pdfDocument.cancelSearch(!pdfDocument.searching)
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
                        toolbar.noticeShow(qsTrId("sailfish-office-la-notice-anno-text"))
                        textTool.first = false
                    }
                }
                IconButton {
                    id: textButton
                    anchors.centerIn: parent
                    highlighted: pressed || textTool.pressed || row.activeItem === textTool
                    icon.source: row.activeItem === textTool ? "image://theme/icon-m-annotation-selected"
                                                             : "image://theme/icon-m-annotation"
                    onClicked: textTool.clicked(mouse)
                }
                MouseArea {
                    parent: row.activeItem === textTool ? view : null
                    anchors.fill: parent
                    onClicked: {
                        var annotation = textComponent.createObject(textTool)
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
                    Component {
                        id: textComponent
                        PDF.TextAnnotation { }
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
                        toolbar.noticeShow(qsTrId("sailfish-office-la-notice-anno-highlight"))
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
                    icon.source: row.activeItem === highlightTool ? "image://theme/icon-m-edit-selected"
                                                                  : "image://theme/icon-m-edit"
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
                id: linkBack
                width: row.itemWidth
                height: parent.height
                highlighted: pressed || backButton.pressed
                opacity: view.canMoveBack ? 1. : 0.
                visible: opacity > 0
                Behavior on opacity { FadeAnimation{ duration: 400 } }
                IconButton {
                    id: backButton
                    anchors.centerIn: parent
                    highlighted: pressed || linkBack.pressed
                    icon.source: "image://theme/icon-m-back"
                    onClicked: linkBack.clicked(mouse)
                }
                onClicked: {
                    row.toggle(linkBack)
                    view.moveBack()
                    toolbar.hide()
                }
            }
            BackgroundItem {
                id: rotateTool

                width: row.itemWidth
                height: parent.height
                highlighted: pressed || rotateButton.pressed
                onClicked: view.rotate()
                IconButton {
                    id: rotateButton
                    anchors.centerIn: parent
                    highlighted: pressed || row.activeItem === rotateTool
                    icon.source: row.activeItem === rotateTool ? "image://theme/icon-m-rotate-right-selected"
                                                               : "image://theme/icon-m-rotate-right"
                    onClicked: view.rotate()
                }
            }
            BackgroundItem {
                id: pageCount
                width: screen.sizeCategory <= Screen.Medium
                       ? Math.max(toolbar.width / row.nVisibleChildren, Screen.width / 4)
                       : toolbar.width / row.nVisibleChildren
                height: parent.height
                Label {
                    id: pageLabel
                    anchors.centerIn: parent
                    width: Math.min(parent.width - Theme.paddingSmall, implicitWidth)
                    fontSizeMode: Text.HorizontalFit
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

    Component {
        id: contextMenuLinksComponent
        PDFContextMenuLinks { }
    }

    Component {
        id: contextMenuTextComponent
        PDFContextMenuText { }
    }

    Component {
        id: contextMenuHighlightComponent
        PDFContextMenuHighlight { }
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
