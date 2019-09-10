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
    id: page

    property var _settings // Handle save and restore the view settings using PDFStorage
    property ContextMenu contextMenuLinks
    property ContextMenu contextMenuText
    property ContextMenu contextMenuHighlight

    documentItem: view
    busy: (!doc.loaded && !doc.failure) || doc.searching
    error: doc.failure
    source: doc.source

    onSourceChanged: {
        var remorsePopup = deleteButton.remorsePopup
        if (remorsePopup && remorsePopup.active) remorsePopup.trigger()
    }

    function savePageSettings() {
        if (!rememberPositionConfig.value || doc.failure || doc.locked) {
            return
        }
        
        if (!_settings) {
            _settings = new PDFStorage.Settings(doc.source)
        }
        var last = view.getPagePosition()
        _settings.setLastPage(last[0] + 1, last[1], last[2], view.itemWidth)
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
                    _settings = new PDFStorage.Settings(doc.source)
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

    Loader {
        parent: page
        active: doc.failure || doc.locked
        sourceComponent: placeholderComponent
        anchors.verticalCenter: parent.verticalCenter
    }

    PDFView {
        id: view

        // for cover state
        property bool contentAvailable: doc.loaded && !(doc.failure || doc.locked)
        property alias title: page.title
        property alias mimeType: page.mimeType

        anchors {
            fill: parent
            bottomMargin: toolbar.offset
        }
        document: doc
        header: header
        clip: anchors.bottomMargin > 0

        enabled: doc.loaded
        opacity: enabled ? 1.0 : 0.0
        Behavior on opacity { FadeAnimator { duration: 400 }}

        onCanMoveBackChanged: if (canMoveBack) toolbar.show()

        onLinkClicked: {
            if (!contextMenuLinks) {
                contextMenuLinks = contextMenuLinksComponent.createObject(page)
            }
            contextMenuLinks.url = linkTarget
            hook.showMenu(contextMenuLinks)
        }

        onAnnotationClicked: {
            switch (annotation.type) {
            case PDF.Annotation.Highlight:
                if (!contextMenuHighlight) {
                    contextMenuHighlight = contextMenuHighlightComponent.createObject(page)
                }
                contextMenuHighlight.annotation = annotation
                hook.showMenu(contextMenuHighlight)
                break
            case PDF.Annotation.Caret:
            case PDF.Annotation.Text:
                doc.edit(annotation)
                break
            default:
            }
        }

        onAnnotationLongPress: {
            switch (annotation.type) {
            case PDF.Annotation.Highlight:
                if (!contextMenuHighlight) {
                    contextMenuHighlight = contextMenuHighlightComponent.createObject(page)
                }
                contextMenuHighlight.annotation = annotation
                hook.showMenu(contextMenuHighlight)
                break
            case PDF.Annotation.Caret:
            case PDF.Annotation.Text:
                if (!contextMenuText) {
                    contextMenuText = contextMenuTextComponent.createObject(page)
                }
                contextMenuText.annotation = annotation
                hook.showMenu(contextMenuText)
                break
            default:
            }
        }

        onLongPress: {
            if (!contextMenuText) {
                contextMenuText = contextMenuTextComponent.createObject(page)
            }
            contextMenuText.at = pressAt
            contextMenuText.annotation = null
            hook.showMenu(contextMenuText)
        }

        PullDownMenu {
            MenuItem {
                id: deleteButton
                //% "Delete"
                text: qsTrId("sailfish-office-me-delete")
                property Item remorsePopup
                readonly property bool remorseActive: remorsePopup && remorsePopup.active
                function remorseAction(text, action) {
                    if (!remorsePopup) {
                        remorsePopup = remorsePopupComponent.createObject(page)
                    }
                    if (!remorsePopup.active) {
                        remorsePopup.execute(text, action)
                    }
                }

                onClicked: {
                    //: Deleting file after timeout.
                    //% "Deleting"
                    remorseAction(qsTrId("sailfish-office-la-deleting"), function() {
                        page.provider.deleteFile(page.source)
                        pageStack.pop()
                    })
                }
                Component {
                    id: remorsePopupComponent
                    RemorsePopup {}
                }
            }
            MenuItem {
                //% "Share"
                text: qsTrId("sailfish-office-me-share")
                onClicked: {
                    pageStack.animatorPush("Sailfish.TransferEngine.SharePage",
                                           {
                                               "source": page.source,
                                               "mimeType": page.mimeType,
                                               "serviceFilter": ["e-mail"]
                                           })
                }
            }
        }

        DocumentHeader {
            id: header
            page: page
            indexCount: doc.pageCount
            width: page.width
            x: view.contentX
        }
    }

    ToolBar {
        id: toolbar

        property bool active: indexButton.highlighted
                              || linkBack.visible
                              || search.highlighted
                              || search.active
                              || textTool.highlighted
                              || highlightTool.highlighted
                              || view.selection.selected

        property Item activeItem

        function toggle(item) {
            if (toolbar.notice) toolbar.notice.hide()
            view.selection.unselect()
            if (toolbar.activeItem === item) {
                toolbar.activeItem = null
            } else {
                toolbar.activeItem = item
            }
        }

        flickable: view
        anchors.top: view.bottom
        enabled: doc.loaded
        opacity: enabled ? 1.0 : 0.0
        Behavior on opacity { FadeAnimator { duration: 400 }}

        forceHidden: doc.failure || doc.locked
                     || (contextMenuLinks && contextMenuLinks.active)
                     || (contextMenuHighlight && contextMenuHighlight.active)
                     || (contextMenuText && contextMenuText.active)
        autoShowHide: !toolbar.active

        Connections {
            target: view.selection
            onSelectedChanged: if (view.selection.selected) toolbar.show()
        }


        SearchBarItem {
            id: search
            width: textTool.width
            expandedWidth: page.width
            height: parent.height

            searching: doc.searching
            searchProgress: doc.searchModel ? doc.searchModel.fraction : 0.
            matchCount: doc.searchModel ? doc.searchModel.count : -1

            onRequestSearch: doc.search(text, view.currentPage - 1)
            onRequestPreviousMatch: view.prevSearchMatch()
            onRequestNextMatch: view.nextSearchMatch()
            onRequestCancel: doc.cancelSearch(!doc.searching)
            onClicked: toolbar.toggle(search)
        }

        IconButton {
            id: textTool
            property bool first: true

            onClicked: {
                toolbar.toggle(textTool)
                if (textTool.first) {
                    //% "Tap where you want to add a note"
                    noticeShow(qsTrId("sailfish-office-la-notice-anno-text"))
                    textTool.first = false
                }
            }

            anchors.verticalCenter: parent.verticalCenter
            highlighted: pressed || toolbar.activeItem === textTool
            icon.source: toolbar.activeItem === textTool ? "image://theme/icon-m-annotation-selected"
                                                         : "image://theme/icon-m-annotation"
            MouseArea {
                parent: toolbar.activeItem === textTool ? view : null
                anchors.fill: parent
                onClicked: {
                    var annotation = textComponent.createObject(textTool)
                    var pt = Qt.point(view.contentX + mouse.x, view.contentY + mouse.y)
                    doc.create(annotation,
                               function() {
                                   var at = view.getPositionAt(pt)
                                   annotation.attachAt(doc,
                                                       at[0], at[2], at[1])
                               })
                    toolbar.toggle(textTool)
                }
                Component {
                    id: textComponent
                    PDF.TextAnnotation { }
                }
            }
        }

        IconButton {
            id: highlightTool
            property bool first: true

            function highlightSelection() {
                var anno = highlightComponent.createObject(highlightTool)
                anno.color = highlightColorConfig.value
                anno.style = highlightStyleConfig.toEnum(highlightStyleConfig.value)
                anno.attach(doc, view.selection)
                toolbar.hide()
            }

            onClicked: {
                if (view.selection.selected) {
                    highlightSelection()
                    view.selection.unselect()
                    return
                }
                toolbar.toggle(highlightTool)
                if (first) {
                    //% "Tap and move your finger over the area"
                    noticeShow(qsTrId("sailfish-office-la-notice-anno-highlight"))
                    first = false
                }
            }

            Component {
                id: highlightComponent
                PDF.HighlightAnnotation { }
            }

            anchors.verticalCenter: parent.verticalCenter
            highlighted: pressed || toolbar.activeItem === highlightTool
            icon.source: toolbar.activeItem === highlightTool ? "image://theme/icon-m-edit-selected"
                                                              : "image://theme/icon-m-edit"
            MouseArea {
                parent: toolbar.activeItem === highlightTool ? view : null
                anchors.fill: parent
                preventStealing: true
                onPressed: {
                    var pt = mapToItem(view.canvas, mouse.x, mouse.y)
                    view.selection.selectAt(pt)
                }
                onPositionChanged: {
                    var pt = mapToItem(view.canvas, mouse.x, mouse.y)
                    if (view.selection.count < 1) {
                        view.selection.selectAt(pt)
                    } else {
                        view.selection.handle2 = pt
                    }
                }
                onReleased: {
                    if (view.selection.selected) highlightTool.highlightSelection()
                    toolbar.toggle(highlightTool)
                }
                Binding {
                    target: view
                    property: "selectionDraggable"
                    value: toolbar.activeItem !== highlightTool
                }
            }
        }

        IconButton {
            id: linkBack
            anchors.verticalCenter: parent.verticalCenter
            opacity: view.canMoveBack ? 1. : 0.
            visible: opacity > 0
            Behavior on opacity { FadeAnimator { duration: 400 } }
            icon.source: "image://theme/icon-m-back"
            onClicked: {
                toolbar.toggle(linkBack)
                view.moveBack()
                toolbar.hide()
            }
        }

        IndexButton {
            id: indexButton
            onClicked: {
                toolbar.toggle(indexButton)
                var obj = pageStack.animatorPush(Qt.resolvedUrl("PDFDocumentToCPage.qml"),
                                                 { tocModel: doc.tocModel, pageCount: doc.pageCount })

                obj.pageCompleted.connect(function(page) {
                    page.onPageSelected.connect(function(pageNumber) { view.goToPage(pageNumber) } )
                })
            }

            index: view.currentPage
            count: doc.pageCount
            allowed: !doc.failure && !doc.locked
        }
    }

    PDF.Document {
        id: doc
        source: page.source
        autoSavePath: page.source

        function create(annotation, callback) {
            var isText = (annotation.type == PDF.Annotation.Text
                          || annotation.type == PDF.Annotation.Caret)
            var obj = pageStack.animatorPush(Qt.resolvedUrl("PDFAnnotationNew.qml"),
                                             {"isTextAnnotation": isText})
            obj.pageCompleted.connect(function(dialog) {
                dialog.accepted.connect(function() {
                    annotation.contents = dialog.text
                })
                if (callback !== undefined) dialog.accepted.connect(callback)
            })
        }
        function edit(annotation) {
            var obj = pageStack.animatorPush(Qt.resolvedUrl("PDFAnnotationEdit.qml"),
                                             {"annotation": annotation})
            obj.pageCompleted.connect(function(edit) {
                edit.remove.connect(function() {
                    annotation.remove()
                    pageStack.pop()
                })
            })
        }
    }

    property Notification notice

    function noticeShow(message) {
        if (!notice) {
            notice = noticeComponent.createObject(toolbar)
        }
        notice.show(message)
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

    Component {
        id: placeholderComponent

        Column {
            width: page.width

            InfoLabel {
                text: doc.failure ? //% "Broken file"
                                    qsTrId("sailfish-office-me-broken-pdf")
                                  : //% "Locked file"
                                    qsTrId("sailfish-office-me-locked-pdf")
            }

            InfoLabel {
                font.pixelSize: Theme.fontSizeLarge
                color: Theme.rgba(Theme.highlightColor, Theme.opacityLow)
                text: doc.failure ? //% "Cannot read the PDF document"
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

                visible: doc.locked
                x: Theme.horizontalPageMargin
                width: parent.width - 2*x
                EnterKey.enabled: text
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: {
                    focus = false
                    doc.requestUnLock(text)
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
        onTriggered: linkArea.sourceSize = Qt.size(page.width, pdfCanvas.height)
    }
}
