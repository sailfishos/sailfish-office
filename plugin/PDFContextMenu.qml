/****************************************************************************************
  **
  ** Copyright (C) 2013-2016 Jolla Ltd., Damien Caliste
  ** Contact: Martin Jones <martin.jones@jollamobile.com>
  ** Contact: Damien Caliste <dcaliste@free.fr>
  ** All rights reserved.
  ** 
  ** You may use this file under the terms of BSD license as follows:
  **
  ** Redistribution and use in source and binary forms, with or without
  ** modification, are permitted provided that the following conditions are met:
  **     * Redistributions of source code must retain the above copyright
  **       notice, this list of conditions and the following disclaimer.
  **     * Redistributions in binary form must reproduce the above copyright
  **       notice, this list of conditions and the following disclaimer in the
  **       documentation and/or other materials provided with the distribution.
  **     * Neither the name of the Jolla Ltd nor the
  **       names of its contributors may be used to endorse or promote products
  **       derived from this software without specific prior written permission.
  ** 
  ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ** ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  ** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  ** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  ** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  **
  ****************************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0

Rectangle {
    id: root

    property alias title: labelTitle.text
    property alias message: labelMessage.text
    default property alias children: contentColumn.data

    readonly property bool landscape: width > height
    property Item _highlightedItem

    gradient: Gradient {
        GradientStop { position: 0.0; color: Theme.highlightDimmerColor }
        GradientStop { position: 1.0; color: Theme.rgba(Theme.highlightDimmerColor, .91) }
    }
    opacity: 0.
    Behavior on opacity { FadeAnimation { duration: 300 } }
    visible: opacity > 0.
    anchors.fill: parent
        
    MouseArea {
        anchors.fill: parent
        preventStealing: true

        onPressed: _highlightMenuItem(mouse.y - contentColumn.y)
        onPositionChanged: _highlightMenuItem(mouse.y - contentColumn.y)
        onCanceled: _highlightMenuItem(null)
        onReleased: {
            if (_highlightedItem !== null) {
                _highlightedItem.down = false
                _highlightedItem.clicked()
            }
            root.opacity = 0.
        }
    }

    Column {
        spacing: Theme.paddingMedium
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingLarge * 2
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - Theme.paddingLarge * 2
        InfoLabel {
            id: labelTitle
            elide: Text.ElideRight
            wrapMode: Text.Wrap
            maximumLineCount: 2
            color: Theme.highlightColor
            opacity: .6
        }
        InfoLabel {
            id: labelMessage
            font.pixelSize: Theme.fontSizeMedium
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            maximumLineCount: landscape ? 1 : 4
            color: Theme.highlightColor
            opacity: .6
        }
    }

    Column {
        id: contentColumn

        anchors.bottom: parent.bottom
        anchors.bottomMargin: landscape ? Theme.paddingLarge : Theme.itemSizeSmall
        width: parent.width
    }

    function _highlightMenuItem(yPos) {
        var xPos = width / 2
        var child = contentColumn.childAt(xPos, yPos)
        if (!child) {
            _setHighlightedItem(null)
            return
        }
        var parentItem
        while (child) {
            if (child && child.hasOwnProperty("__silica_menuitem") && child.enabled) {
                _setHighlightedItem(child)
                break
            }
            parentItem = child
            yPos = parentItem.mapToItem(child, xPos, yPos).y
            child = parentItem.childAt(xPos, yPos)
        }
    }

    function _setHighlightedItem(item) {
        if (item === _highlightedItem) {
            return
        }
        if (_highlightedItem) {
            _highlightedItem.down = false
        }
        _highlightedItem = item
        if (_highlightedItem) {
            _highlightedItem.down = true
        }
    }
}
