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
import Sailfish.Silica.private 1.0
import Sailfish.Office.PDF 1.0 as PDF
import Nemo.Configuration 1.0

DocumentFlickable {
    id: root

    property alias itemWidth: pdfCanvas.width
    property alias itemHeight: pdfCanvas.height

    property Item header
    property alias canvas: pdfCanvas
    property alias document: pdfCanvas.document
    property int currentPage: !quickScrollAnimation.running
                              ? pdfCanvas.currentPage : quickScrollAnimation.pageTo
    property alias selection: pdfSelection
    property alias selectionDraggable: selectionView.draggable
    property bool canMoveBack: (_contentYAtGotoLink >= 0)

    property QtObject _feedbackEffect

    property int _pageAtLinkTarget
    property int _pageAtGotoLink
    property real _contentXAtGotoLink: -1.
    property real _contentYAtGotoLink: -1.

    property int _searchIndex

    signal clicked()
    signal linkClicked(string linkTarget, Item hook)
    signal selectionClicked(variant selection, Item hook)
    signal annotationClicked(variant annotation, Item hook)
    signal annotationLongPress(variant annotation, Item hook)
    signal longPress(point pressAt, Item hook)
    signal pageSizesReady()
    signal updateSize(real newWidth, real newHeight)

    function clamp(value) {
        var maximumZoom = Math.min(Screen.height, Screen.width) * maxZoomLevelConfig.value
        return Math.max(width, Math.min(value, maximumZoom))
    }

    function zoom(amount, center) {
        var oldWidth = pdfCanvas.width
        var oldHeight = pdfCanvas.height
        var oldContentX = contentX
        var oldContentY = contentY

        pdfCanvas.width = clamp(pdfCanvas.width * amount)

        /* One cannot use += here because changing contentX will change contentY
           to adjust to new height, so we use saved values. */
        contentX = oldContentX + (center.x * pdfCanvas.width / oldWidth) - center.x
        contentY = oldContentY + (center.y * pdfCanvas.height / oldHeight) - center.y

        _contentXAtGotoLink = -1.
        _contentYAtGotoLink = -1.
    }

    onClicked: {
        if (zoomed) {
            var scale = pdfCanvas.width / width
            zoomOutContentYAnimation.to = Math.max(0, Math.min(contentHeight - height,
                                                               (contentY + height/2) / scale - height/2))
            zoomOutAnimation.start()
        }
    }

    function adjust() {
        var oldWidth = pdfCanvas.width
        var oldHeight = pdfCanvas.height
        var oldContentX = contentX
        var oldContentY = contentY

        pdfCanvas.width = zoomed ? clamp(pdfCanvas.width) : width

        contentX = oldContentX * pdfCanvas.width / oldWidth
        if (!contextHook.active) {
            contentY = oldContentY * pdfCanvas.height / oldHeight
        }
    }

    function moveToSearchMatch(index) {
        if (index < 0 || index >= searchDisplay.count) return

        _searchIndex = index

        var match = searchDisplay.itemAt(index)
        var cX = match.x + match.width / 2. - width / 2.
        cX = Math.max(0, Math.min(cX, pdfCanvas.width - width))
        var cY = match.y + match.height / 2. - height / 2.
        cY = Math.max(0, Math.min(cY, pdfCanvas.height - height))

        scrollTo(Qt.point(cX, cY), match.page, match)
    }

    function nextSearchMatch() {
        if (_searchIndex + 1 >= searchDisplay.count) {
            moveToSearchMatch(0)
        } else {
            moveToSearchMatch(_searchIndex + 1)
        }
    }

    function prevSearchMatch() {
        if (_searchIndex < 1) {
            moveToSearchMatch(searchDisplay.count - 1)
        } else {
            moveToSearchMatch(_searchIndex - 1)
        }
    }

    function scrollTo(pt, pageId, focusItem) {
        if ((pt.y < root.contentY + root.height && pt.y > root.contentY - root.height)
                && (pt.x < root.contentX + root.width && pt.x > root.contentX - root.width)) {
            scrollX.to = pt.x
            scrollY.to = pt.y
            scrollAnimation.focusItem = (focusItem !== undefined) ? focusItem : null
            scrollAnimation.start()
        } else {
            var deltaY = pt.y - root.contentY
            if (deltaY < 0) {
                deltaY = Math.max(deltaY / 2., -root.height / 2.)
            } else {
                deltaY = Math.min(deltaY / 2., root.height / 2.)
            }
            leaveX.to = (root.contentX + pt.x) / 2
            leaveY.to = root.contentY + deltaY
            returnX.to = pt.x
            returnY.from = pt.y - deltaY
            returnY.to = pt.y
            quickScrollAnimation.pageTo = pageId
            quickScrollAnimation.focusItem = (focusItem !== undefined) ? focusItem : null
            quickScrollAnimation.start()
        }
    }

    function moveBack() {
        if (!canMoveBack) {
            return
        }

        scrollTo(Qt.point(_contentXAtGotoLink, _contentYAtGotoLink), _pageAtGotoLink)

        _pageAtLinkTarget = 0
        _contentXAtGotoLink = -1.
        _contentYAtGotoLink = -1.
    }

    pinchArea.enabled: false // TODO: remove duplicate
    contentWidth: pdfCanvas.width
    contentHeight: pdfCanvas.height + header.height

    SequentialAnimation {
        id: focusAnimation

        property Item targetItem
        NumberAnimation { target: focusAnimation.targetItem; property: "scale"; duration: 200; to: 3.; easing.type: Easing.InOutCubic }
        NumberAnimation { target: focusAnimation.targetItem; property: "scale"; duration: 200; to: 1.; easing.type: Easing.InOutCubic }
    }
    SequentialAnimation {
        id: scrollAnimation

        property Item focusItem
        ParallelAnimation {
            NumberAnimation { id: scrollX; target: root; property: "contentX"; duration: 300; easing.type: Easing.InOutQuad }
            NumberAnimation { id: scrollY; target: root; property: "contentY"; duration: 300; easing.type: Easing.InOutQuad }
        }
        ScriptAction {
            script: {
                if (scrollAnimation.focusItem) {
                    focusAnimation.targetItem = scrollAnimation.focusItem
                    focusAnimation.start()
                }
            }
        }
    }
    SequentialAnimation {
        id: quickScrollAnimation

        property int pageTo
        property Item focusItem

        ParallelAnimation {
            NumberAnimation { id: leaveX; target: root; property: "contentX"; duration: 300; easing.type: Easing.InQuad }
            NumberAnimation { id: leaveY; target: root; property: "contentY"; duration: 300; easing.type: Easing.InQuad }
            NumberAnimation { target: root; property: "opacity"; duration: 300; to: 0.; easing.type: Easing.InQuad }
        }
        PauseAnimation { duration: 100 }
        ParallelAnimation {
            NumberAnimation { id: returnX; target: root; property: "contentX"; duration: 300; easing.type: Easing.OutQuad }
            NumberAnimation { id: returnY; target: root; property: "contentY"; duration: 300; easing.type: Easing.OutQuad }
            NumberAnimation { target: root; property: "opacity"; duration: 300; to: 1.; easing.type: Easing.OutQuad }
        }
        ScriptAction {
            script: {
                if (quickScrollAnimation.focusItem) {
                    focusAnimation.targetItem = quickScrollAnimation.focusItem
                    focusAnimation.start()
                }
            }
        }
    }
    NumberAnimation {
        id: selectionOffset

        property real start

        duration: 200
        easing.type: Easing.InOutCubic
        target: root
        property: "contentY"
    }

    // Ensure proper zooming level when device is rotated.
    onWidthChanged: adjust()
    Component.onCompleted: {
        // Avoid hard dependency to feedback
        _feedbackEffect = Qt.createQmlObject("import QtQuick 2.0; import QtFeedback 5.0; ThemeEffect { effect: ThemeEffect.PressWeak }",
                                             root, 'ThemeEffect')
        if (_feedbackEffect && !_feedbackEffect.supported) {
            _feedbackEffect = null
        }
    }

    Connections {
        target: document
        onSearchModelChanged: moveToFirstMatch.done = false
    }

    Connections {
        id: moveToFirstMatch

        property bool done

        target: document.searchModel
        onCountChanged: {
            if (done) return

            moveToSearchMatch(0)
            done = true
        }
    }

    PDF.Selection {
        id: pdfSelection

        property bool dragging: drag1.pressed || drag2.pressed
        property bool selected: count > 0

        canvas: pdfCanvas
        wiggle: Theme.itemSizeSmall / 2

        onDraggingChanged: {
            if (dragging) {
                if (!selectionOffset.running)
                    selectionOffset.start = root.contentY

                // Limit offset when being at the bottom of the view.
                selectionOffset.to = selectionOffset.start
                        + Math.min(Theme.itemSizeSmall,
                                   Math.max(0, root.itemHeight - root.height - pdfCanvas.y - root.contentY))
                // Limit offset when being at the top of screen
                selectionOffset.to =
                        Math.max(root.contentY,
                                 Math.min(selectionOffset.to,
                                          (drag1.pressed ? handle1.y : handle2.y)
                                          - Theme.itemSizeSmall / 2)
                                 )
            } else {
                selectionOffset.to = selectionOffset.start
            }
            selectionOffset.restart()

            // Copy selection to clipboard when dragging finishes
            if (!dragging) Clipboard.text = text
        }
        // Copy selection to clipboard on first selection
        onSelectedChanged: if (selected) Clipboard.text = text
    }

    PDF.Canvas {
        id: pdfCanvas

        property bool _pageSizesReady

        objectName: "application"

        y: header.height
        width: root.width
        spacing: Theme.paddingLarge
        flickable: root
        linkWiggle: Theme.itemSizeMedium / 2
        linkColor: Theme.highlightColor
        pagePlaceholderColor: "white"

        onPageLayoutChanged: {
            if (!_pageSizesReady) {
                _pageSizesReady = true
                root.pageSizesReady()
            }
        }

        onCurrentPageChanged: {
            // If the document is moved than more than one page
            // the back move is cancelled.
            if (_pageAtLinkTarget > 0
                    && !scrollAnimation.running
                    && !quickScrollAnimation.running
                    && (currentPage > _pageAtLinkTarget + 1
                        || currentPage < _pageAtLinkTarget - 1)) {
                _pageAtLinkTarget = 0
                _contentXAtGotoLink = -1.
                _contentYAtGotoLink = -1.
            }
        }

        PinchArea {
            anchors.fill: parent
            enabled: !pageStack.dragInProgress
            onPinchUpdated: {
                var newCenter = mapToItem(pdfCanvas, pinch.center.x, pinch.center.y)
                root.zoom(1.0 + (pinch.scale - pinch.previousScale), newCenter)
            }
            onPinchFinished: root.returnToBounds()

            PDF.LinkArea {
                id: linkArea

                anchors.fill: parent
                onClickedBoxChanged: {
                    if (clickedBox.width > 0) {
                        contextHook.setTarget(clickedBox.y, clickedBox.height)
                    }
                }

                canvas: pdfCanvas
                selection: pdfSelection

                onLinkClicked: root.linkClicked(linkTarget, contextHook)
                onGotoClicked: {
                    var pt = root.contentAt(page - 1, top, left,
                                            Theme.paddingLarge, Theme.paddingLarge)
                    _pageAtLinkTarget = page
                    _pageAtGotoLink = pdfCanvas.currentPage
                    _contentXAtGotoLink = root.contentX
                    _contentYAtGotoLink = root.contentY
                    scrollTo(pt, page)
                }
                onSelectionClicked: root.selectionClicked(selection, contextHook)
                onAnnotationClicked: root.annotationClicked(annotation, contextHook)
                onClicked: root.clicked()
                onAnnotationLongPress: root.annotationLongPress(annotation, contextHook)
                onLongPress: {
                    contextHook.setTarget(pressAt.y, Theme.itemSizeSmall / 2)
                    root.longPress(pressAt, contextHook)
                }
            }
        }

        Rectangle {
            x: linkArea.clickedBox.x
            y: linkArea.clickedBox.y
            width: linkArea.clickedBox.width
            height: linkArea.clickedBox.height
            radius: Theme.paddingSmall
            color: Theme.highlightColor
            opacity: linkArea.pressed ? 0.75 : 0.
            visible: opacity > 0.
            Behavior on opacity { FadeAnimator { duration: 100 } }
        }

        Repeater {
            id: searchDisplay

            model: pdfCanvas.document.searchModel
            delegate: Rectangle {
                property int page: model.page
                property rect pageRect: model.rect
                property rect match: pdfCanvas.fromPageToItem(page, pageRect)

                Connections {
                    target: pdfCanvas
                    onPageLayoutChanged: match = pdfCanvas.fromPageToItem(page, pageRect)
                }

                opacity: 0.5
                color: Theme.highlightColor
                x: match.x - Theme.paddingSmall / 2
                y: match.y - Theme.paddingSmall / 4
                width: match.width + Theme.paddingSmall
                height: match.height + Theme.paddingSmall / 2
            }
        }

        PDFSelectionView {
            id: selectionView

            model: pdfSelection
            flickable: root
            dragHandle1: drag1.pressed
            dragHandle2: drag2.pressed
            onVisibleChanged: if (visible && _feedbackEffect) _feedbackEffect.play()
        }
        PDFSelectionDrag {
            id: drag1

            visible: pdfSelection.selected && selectionView.draggable
            flickable: root
            handle: pdfSelection.handle1
            onDragged: pdfSelection.handle1 = at
        }
        PDFSelectionDrag {
            id: drag2

            visible: pdfSelection.selected && selectionView.draggable
            flickable: root
            handle: pdfSelection.handle2
            onDragged: pdfSelection.handle2 = at
        }
        ContextMenuHook {
            id: contextHook

            Connections {
                target: linkArea
                onPositionChanged: {
                    if (contextHook.active) {
                        var local = linkArea.mapToItem(contextHook, at.x, at.y)
                        contextHook.positionChanged(Qt.point(local.x, local.y))
                    }
                }
                onReleased: if (contextHook.active) contextHook.released(true)
            }
        }
    }

    children: [
        HorizontalScrollDecorator { color: Theme.highlightDimmerColor },
        VerticalScrollDecorator { color: Theme.highlightDimmerColor }
    ]

    ConfigurationValue {
        id: maxZoomLevelConfig

        key: "/apps/sailfish-office/settings/maxZoomLevel"
        defaultValue: 10.
    }

    function pageRectangle(pageNumber) {
        var rect = pdfCanvas.pageRectangle( pageNumber )
        rect.y = rect.y + pdfCanvas.y
        return rect
    }

    function contentAt(pageNumber, top, left, topSpacing, leftSpacing) {
        var rect = pageRectangle( pageNumber )

        var scrollX, scrollY
        // Adjust horizontal position if required.
        scrollX = root.contentX
        if (left !== undefined && left >= 0.) {
            scrollX = rect.x + left * rect.width - ( leftSpacing !== undefined ? leftSpacing : 0.)
        }
        if (scrollX > contentWidth - width) {
            scrollX = contentWidth - width
        }
        // Adjust vertical position.
        scrollY = rect.y + (top === undefined ? 0. : top * rect.height) - ( topSpacing !== undefined ? topSpacing : 0.)
        if (scrollY > contentHeight - height) {
            scrollY = contentHeight - height
        }
        return Qt.point(Math.max(0, scrollX), Math.max(0, scrollY))
    }
    function goToPage(pageNumber, top, left, topSpacing, leftSpacing) {
        var pt = contentAt(pageNumber, top, left, topSpacing, leftSpacing)
        contentX = pt.x
        contentY = pt.y
    }
    // This function is the inverse of goToPage(), returning (pageNumber, top, left).
    function getPagePosition() {
        // Find the page on top
        var i = currentPage - 1
        var rect = pageRectangle( i )
        while (rect.y > contentY && i > 0) {
            rect = pageRectangle( --i )
        }
        var top  = (contentY - rect.y) / rect.height
        var left = (contentX - rect.x) / rect.width
        return [i, top, left]
    }
    function getPositionAt(at) {
        // Find the page that contains at
        var i = Math.max(0, currentPage - 2)
        var rect = pageRectangle( i )
        while ((rect.y + rect.height) < at.y
               && i < pdfCanvas.document.pageCount) {
            rect = pageRectangle( ++i )
        }
        var top  = Math.max(0, at.y - rect.y) / rect.height
        var left = (at.x - rect.x) / rect.width
        return [i, top, left]
    }

    ParallelAnimation {
        id: zoomOutAnimation

        NumberAnimation {
            target: pdfCanvas
            property: "width"
            to: root.width
            easing.type: Easing.InOutQuad
            duration: 200
        }
        NumberAnimation {
            target: root
            properties: "contentX"
            to: 0
            easing.type: Easing.InOutQuad
            duration: 200
        }
        NumberAnimation {
            id: zoomOutContentYAnimation

            target: root
            properties: "contentY"
            easing.type: Easing.InOutQuad
            duration: 200
        }
    }
}
