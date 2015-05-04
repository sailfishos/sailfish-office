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

SilicaFlickable {
    id: base;

    contentWidth: pdfCanvas.width;
    contentHeight: pdfCanvas.height;

    property alias itemWidth: pdfCanvas.width;
    property alias itemHeight: pdfCanvas.height;
    property alias document: pdfCanvas.document;
    property alias currentPage: pdfCanvas.currentPage
    property alias searchModel: searchDisplay.model

    property bool scaled: pdfCanvas.width != width;

    signal clicked();
    signal updateSize(real newWidth, real newHeight);

    function clamp(value) {
        if (value < width) {
            return width;
        }

        if (value > width * 2.5) {
            return width * 2.5;
        }

        return value;
    }

    function zoom(amount, center) {
        var oldWidth = pdfCanvas.width;
        var oldHeight = pdfCanvas.height
        var oldContentX = contentX
        var oldContentY = contentY

        pdfCanvas.width = clamp(pdfCanvas.width * amount);

        /* One cannot use += here because changing contentX will change contentY
           to adjust to new height, so we use saved values. */
        contentX = oldContentX + (center.x * pdfCanvas.width / oldWidth) - center.x
        contentY = oldContentY + (center.y * pdfCanvas.height / oldHeight) - center.y
    }

    function adjust() {
        var oldWidth = pdfCanvas.width
        var oldHeight = pdfCanvas.height
        var oldContentX = contentX
        var oldContentY = contentY

        pdfCanvas.width = scaled ? clamp(pdfCanvas.width) : width

        contentX = oldContentX * pdfCanvas.width / oldWidth
        contentY = oldContentY * pdfCanvas.height / oldHeight
    }

    // Ensure proper zooming level when device is rotated.
    onWidthChanged: adjust()

    PDF.Canvas {
        id: pdfCanvas;

        width: base.width;

        spacing: Theme.paddingLarge;
        flickable: base;
        linkColor: Theme.highlightColor;

        PinchArea {
            anchors.fill: parent;
            onPinchUpdated: {
                var newCenter = mapToItem(pdfCanvas, pinch.center.x, pinch.center.y)
                base.zoom(1.0 + (pinch.scale - pinch.previousScale), newCenter);
            }
            onPinchFinished: base.returnToBounds();

            PDF.LinkArea {
                anchors.fill: parent;

                canvas: pdfCanvas;
                onLinkClicked: Qt.openUrlExternally(linkTarget);
                onClicked: base.clicked();
            }
        }

        Repeater {
            id: searchDisplay
            delegate: Rectangle {
                property int page: model.page
                property rect pageRect: model.rect
                property rect match: pdfCanvas.fromPageToItem(page, pageRect)
                Connections {
                    target: pdfCanvas
                    onPageLayoutChanged: match = pdfCanvas.fromPageToItem(page, pageRect)
                }

                opacity: 0.5;
                color: Theme.highlightColor;
                x: match.x - Theme.paddingSmall / 2;
                y: match.y - Theme.paddingSmall / 4;
                width: match.width + Theme.paddingSmall;
                height: match.height + Theme.paddingSmall / 2;
            }
        }
    }

    children: [
        HorizontalScrollDecorator { color: Theme.highlightDimmerColor; },
        VerticalScrollDecorator { color: Theme.highlightDimmerColor; }
    ]

    function goToPage(pageNumber) {
        base.contentY = pdfCanvas.pagePosition( pageNumber );
    }
}
