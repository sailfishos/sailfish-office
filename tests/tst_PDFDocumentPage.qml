/*
 * Copyright (C) 2016-2019 Caliste Damien.
 * Contact: Damien Caliste <dcaliste@free.fr>
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
import QtTest 1.0
import Sailfish.Silica 1.0
import Sailfish.Office 1.0

ApplicationWindow {
    id: window
    property Item documentItem
    property Page _mainPage

    initialPage: Component {
        PDFDocumentPage {
            id: page
            mimeType: "application/pdf"
            Component.onCompleted: window._mainPage = page

            function clickAt(testCase, x, y) {
                clickFeedback.x = x - clickFeedback.width / 2
                clickFeedback.y = y - clickFeedback.height / 2
                clickAnimation.start()
                testCase.tryCompare(clickAnimation, "running", false)
                testCase.mouseClick(page, x, y)
            }

            Rectangle {
                id: clickFeedback
                height: Theme.itemSizeMedium
                width: height
                radius: Theme.itemSizeMedium / 2
                color: Theme.highlightColor
                opacity: 0.
                SequentialAnimation {
                    id: clickAnimation
                    NumberAnimation { target: clickFeedback; property: "opacity"; duration: 500; to: 1.0; easing.type: Easing.InOutCubic }
                    NumberAnimation { target: clickFeedback; property: "opacity"; duration: 500; to: 0.0; easing.type: Easing.InOutCubic }
                }
            }
        }
    }

    TestCase {
        name: "brokenPDF"
        when: _mainPage && windowShown

        function initTestCase() {
            _mainPage.path = Qt.resolvedUrl("data/broken.pdf")
        }
        
        function test_placeholderBroken() {
            tryCompare(_mainPage, "backNavigation", true)
            tryCompare(_mainPage.document, "failure", true)
            tryCompare(_mainPage.placeHolder, "status", Loader.Ready)
        }
    }

    TestCase {
        name: "protectedPDF"
        when: _mainPage && windowShown

        function initTestCase() {
            _mainPage.path = Qt.resolvedUrl("data/protected.pdf")
        }
        
        function test_placeholderBroken() {
            tryCompare(_mainPage, "backNavigation", true)
            tryCompare(_mainPage.document, "failure", false)
            tryCompare(_mainPage.document, "locked", true)
            tryCompare(_mainPage.placeHolder, "status", Loader.Ready)
            keyClick("f")
            keyClick("o")
            keyClick("o")
            keyClick(Qt.Key_Return)
            tryCompare(_mainPage.document, "locked", false)
            tryCompare(_mainPage.placeHolder, "status", Loader.Null)
            tryCompare(_mainPage.documentItem, "contentAvailable", true)
        }
    }

    TestCase {
        name: "basics"
        when: _mainPage && windowShown

        function initTestCase() {
            _mainPage.path = Qt.resolvedUrl("data/sample.pdf")
            tryCompare(_mainPage.documentItem, "contentAvailable", true)
        }

        function test_pages() {
            compare(_mainPage.document.pageCount, 4)
        }

        function test_currentPage() {
            _mainPage.documentItem.itemWidth = window.width * 1.5
            _mainPage.documentItem.adjust()
            _mainPage.documentItem.goToPage(0, 0.25, 0.25)
            tryCompare(_mainPage.documentItem, "currentPage", 1)
            _mainPage.documentItem.goToPage(2, 0.25, 0.25)
            tryCompare(_mainPage.documentItem, "currentPage", 3)
        }
    }

    TestCase {
        name: "urlLink"
        when: _mainPage && windowShown

        property int linkPage: 0
        property point linkPagePoint: Qt.point(0.278, 0.317)

        function clickAt(page, left, top) {
            var pt = _mainPage.documentItem.contentAt(page, top, left)
            var at = Qt.point(pt.x - _mainPage.documentItem.contentX,
                              pt.y - _mainPage.documentItem.contentY)
            mouseClick(_mainPage.documentItem, at.x, at.y)
        }

        function checkMenuOpenAt(page, left, top) {
            verify(_mainPage.contextMenuLinks)
            tryCompare(_mainPage.contextMenuLinks, "active", true)
            var ct = _mainPage.documentItem.mapFromItem(_mainPage.contextMenuLinks, 0, 0)
            compare(ct.x, 0)
            var pt = _mainPage.documentItem.contentAt(page, top, left)
            fuzzyCompare(ct.y, pt.y - _mainPage.documentItem.contentY, 3 * Theme.paddingSmall)
            tryCompare(_mainPage, "open", false)
        }
        function closeMenuAndCheck() {
            verify(_mainPage.contextMenuLinks.active)
            // Discard it.
            mouseClick(_mainPage.documentItem, window.width / 2, 50)
            verify(!_mainPage.open)
            tryCompare(_mainPage.contextMenuLinks, "active", false)
        }

        function initTestCase() {
            _mainPage.path = Qt.resolvedUrl("data/sample.pdf")
            _mainPage.allowedOrientations = Orientation.Portrait
        }

        function cleanupTestCase() {
            _mainPage.allowedOrientations = Orientation.All
        }

        function test_00_setup() {
            tryCompare(_mainPage.documentItem, "contentAvailable", true)
            // Set width and position
            _mainPage.documentItem.itemWidth = window.width * 1.5
            _mainPage.documentItem.adjust()
            _mainPage.documentItem.goToPage(0, 0.25, 0.25)
            var pt = _mainPage.documentItem.contentAt(0, 0.25, 0.25)
            tryCompare(_mainPage.documentItem, "contentX", Math.round(pt.x))
            tryCompare(_mainPage.documentItem, "contentY", Math.round(pt.y))
        }
        
        function test_contextMenu() {
            // Open the context menu.
            clickAt(linkPage, linkPagePoint.x, linkPagePoint.y)
            checkMenuOpenAt(linkPage, linkPagePoint.x, linkPagePoint.y)
            compare(_mainPage.contextMenuLinks.url, "http://helpdesk.rpi.edu/update.do?artcenterkey=325")
        }

        function test_contextMenu_finalize() {
            closeMenuAndCheck()
        }

        function test_deviceRotation() {
            _mainPage.allowedOrientations = Orientation.Portrait
            clickAt(linkPage, linkPagePoint.x, linkPagePoint.y)
            checkMenuOpenAt(linkPage, linkPagePoint.x, linkPagePoint.y)
            _mainPage.allowedOrientations = Orientation.Landscape
            tryCompare(_mainPage, "width", Screen.height)
            checkMenuOpenAt(linkPage, linkPagePoint.x, linkPagePoint.y)
        }

        function test_deviceRotation_finalize() {
            closeMenuAndCheck()
        }
    }

    TestCase {
        name: "gotoLink"
        when: _mainPage && windowShown

        property int gotoPage: 0
        property point gotoPagePoint: Qt.point(0.537, 0.677)
        property int targetPage: 3
        property point targetPagePoint: Qt.point(0.1065, 0.3689)

        function clickAt(page, left, top) {
            var pt = _mainPage.documentItem.contentAt(page, top, left)
            var at = Qt.point(pt.x - _mainPage.documentItem.contentX,
                              pt.y - _mainPage.documentItem.contentY)
            mouseClick(_mainPage.documentItem, at.x, at.y)
        }

        function initTestCase() {
            _mainPage.path = Qt.resolvedUrl("data/sample.pdf")
        }

        function test_00_setup() {
            tryCompare(_mainPage.documentItem, "contentAvailable", true)
            // Set width and position
            _mainPage.documentItem.itemWidth = window.width * 3
            _mainPage.documentItem.adjust()
            _mainPage.documentItem.goToPage(0, 0.50, 0.50)
            var pt = _mainPage.documentItem.contentAt(0, 0.50, 0.50)
            tryCompare(_mainPage.documentItem, "contentX", Math.round(pt.x))
            tryCompare(_mainPage.documentItem, "contentY", Math.round(pt.y))
        }
        
        function test_goto() {
            clickAt(gotoPage, gotoPagePoint.x, gotoPagePoint.y)
            var pt = _mainPage.documentItem.contentAt(targetPage, targetPagePoint.y, targetPagePoint.x)
            verify(_mainPage.toolbar._active)
            tryCompare(_mainPage.documentItem, "scrolling", false)
            fuzzyCompare(_mainPage.documentItem.contentX, Math.round(pt.x), Theme.paddingSmall)
            fuzzyCompare(_mainPage.documentItem.contentY, Math.round(pt.y), Theme.paddingSmall)
        }

        function test_goto_back() {
            mouseClick(_mainPage, 3.5 * window.width / 5, window.height - Theme.itemSizeSmall)
            tryCompare(_mainPage.documentItem, "scrolling", false)
            tryCompare(_mainPage.toolbar, "offset", 0)
            var at = _mainPage.documentItem.getPositionAt(Qt.point(_mainPage.documentItem.contentX, _mainPage.documentItem.contentY))
            compare(at[0], 0)
            fuzzyCompare(at[1], 0.50, 0.01)
            fuzzyCompare(at[2], 0.50, 0.01)
        }
    }

    TestCase {
        id: searchTestCase
        name: "search"
        when: _mainPage && windowShown

        property int matchPage: 1
        property rect match: Qt.rect(0.546236, 0.794508, 0.0332727, 0.0126171)
        property int backMatchPage: 0
        property rect backMatch: Qt.rect(0.723961, 0.312414, 0.0332727, 0.0122314)

        IconButton {
            id: refIcon
            width: icon.width
            height: icon.height
            icon.source: "image://theme/icon-m-left"
        }

        function checkCentredMatch(page, match) {
            tryCompare(_mainPage.documentItem, "scrolling", false)
            var pt = _mainPage.documentItem.contentAt(page, match.y + match.height / 2,
                                                            match.x + match.width / 2)
            fuzzyCompare(_mainPage.documentItem.contentX,
                         Math.round(pt.x - _mainPage.documentItem.width / 2),
                         Theme.paddingSmall)
            fuzzyCompare(_mainPage.documentItem.contentY,
                          Math.round(pt.y - _mainPage.documentItem.height / 2),
                         Theme.paddingSmall)
        }

        function initTestCase() {
            _mainPage.path = Qt.resolvedUrl("data/sample.pdf")
            // Set width and position
            _mainPage.documentItem.itemWidth = window.width * 5
            _mainPage.documentItem.adjust()
            _mainPage.allowedOrientations = Orientation.Portrait
        }

        function cleanupTestCase() {
            _mainPage.toolbar.searchIconized = true
        }

        function test_no_match() {
            _mainPage.toolbar.show()
            tryCompare(_mainPage.toolbar, "offset", _mainPage.toolbar.height)
            _mainPage.clickAt(searchTestCase, 0.5 * window.width / 4, window.height - _mainPage.toolbar.height / 2)
            tryCompare(_mainPage.toolbar, "searchIconized", false)
            verify(!_mainPage.toolbar.autoShowHide)
            keyClick("p")
            keyClick("l")
            keyClick("o")
            keyClick("u")
            keyClick("m")
            keyClick(Qt.Key_Return)
            tryCompare(_mainPage.document, "searching", false)
            verify(_mainPage.document.searchModel)
            compare(_mainPage.document.searchModel.count, 0)
            tryCompare(_mainPage, "height", window.height) // Ensure keyboard is folded
        }

        function test_no_match_finalize() {
            tryCompare(_mainPage.toolbar, "searchIconized", false)
            _mainPage.clickAt(searchTestCase, window.width - refIcon.width / 2 - Theme.horizontalPageMargin, window.height - _mainPage.toolbar.height / 2)
            tryCompare(_mainPage.toolbar, "searchIconized", true)
        }

        function test_match() {
            _mainPage.documentItem.goToPage(0, 0., 0.)
            _mainPage.toolbar.show()
            tryCompare(_mainPage.toolbar, "offset", _mainPage.toolbar.height)
            _mainPage.clickAt(searchTestCase, 0.5 * window.width / 4, window.height - _mainPage.toolbar.height / 2)
            tryCompare(_mainPage.toolbar, "searchIconized", false)
            verify(!_mainPage.toolbar.autoShowHide)
            keyClick("e")
            keyClick("a")
            keyClick("s")
            keyClick("y")
            keyClick(Qt.Key_Return)
            tryCompare(_mainPage.document, "searching", false)
            verify(_mainPage.document.searchModel)
            compare(_mainPage.document.searchModel.count, 2)
            tryCompare(_mainPage, "height", window.height) // Ensure keyboard is folded
            tryCompare(_mainPage.documentItem, "scrolling", false)
            tryCompare(_mainPage.toolbar, "searchIconized", false)
            _mainPage.clickAt(searchTestCase, window.width - Theme.paddingLarge - 1.5 * refIcon.width - Theme.horizontalPageMargin, window.height - _mainPage.toolbar.height / 2)
            checkCentredMatch(matchPage, match)
        }

        function test_match_back_navigate() {
            tryCompare(_mainPage.toolbar, "searchIconized", false)
            _mainPage.clickAt(searchTestCase, window.width - 2 * Theme.paddingLarge - 2.5 * refIcon.width - Theme.horizontalPageMargin, window.height - _mainPage.toolbar.height / 2)
            checkCentredMatch(backMatchPage, backMatch)
        }

        function test_match_reopen() {
            _mainPage.clickAt(searchTestCase, window.width / 2, window.height - _mainPage.toolbar.height / 2)
            tryCompare(_mainPage.toolbar, "searchIconized", false)
            _mainPage.clickAt(searchTestCase, _mainPage.width - refIcon.width / 2 - Theme.horizontalPageMargin, window.height - _mainPage.toolbar.height / 2)
            tryCompare(_mainPage.toolbar, "searchIconized", false)
            compare(_mainPage.toolbar.searchText, "")
        }

        function test_match_reopen_finalize() {
            tryCompare(_mainPage.toolbar, "searchIconized", false)
            _mainPage.clickAt(searchTestCase, window.width - refIcon.width / 2 - Theme.horizontalPageMargin, window.height - _mainPage.toolbar.height / 2)
            tryCompare(_mainPage.toolbar, "searchIconized", true)
            _mainPage.toolbar.hide()
            tryCompare(_mainPage.toolbar, "offset", 0)
        }
    }

/*    TestCase {
        name: "scrolling"
        when: _mainPage && windowShown

        function initTestCase() {
            _mainPage.path = Qt.resolvedUrl("sample.pdf")
        }

        function test_00_check() {
            tryCompare(_mainPage.documentItem, "contentAvailable", true)
        }
        
        function test_simpleScrollDown() {
            var x = window.width / 2
            var y = window.height / 2
            var dx = window.width / 10
            var dy = -window.height / 4
            var duration = 100
            var i
            var dt = 100
            mousePress(_mainPage.documentItem, x, y)
            for (i = 0; i < duration; i += dt) {
                mouseMove(_mainPage.documentItem, x + dx * i / duration, y + dy * i / duration)
                //wait(dt)
            }
            mouseRelease(_mainPage.documentItem, x + dx, y + dy)
            tryCompare(_mainPage.documentItem, "contentX", 0)
            tryCompare(_mainPage.documentItem, "contentY", -dy)
        }

        function test_simpleScrollUp() {
        }

        function test_quickScrollDown() {
        }

        function test_quickScrollUp() {
        }
    }*/
}
