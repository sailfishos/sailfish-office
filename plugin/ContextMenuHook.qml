/*
 * Copyright (C) 2015 Caliste Damien.
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
import Sailfish.Silica 1.0

Item {
    id: hook

    property bool active: _menu ? _menu.active : false
    property int hookHeight
    property alias backgroundColor: background.color
    property alias backgroundOpacity: background.opacity

    property real _flickableContentHeight
    property bool _opened: _menu ? _menu._open : false

    property variant _menu

    // Used to emulate the MouseArea that trigger a ContextMenu
    property bool pressed: true
    property bool preventStealing
    signal positionChanged()
    signal released()

    function showMenu(menu) {
        _menu = menu
        menu.show(hook)
        _flickableContentHeight = _menu._flickable.contentHeight
    }

    // Ensure that flickable position is restored after context menu
    // has been closed. We cannot trust the value that will be restored
    // automatically when the state of the menu changes because the
    // contentHeight of the flickable may have changed in-between due to
    // device rotation for instance.
    on_OpenedChanged: {
        if (!_opened) {
            // Limit the flickable going back to previous y position
            // if the device has been rotated and the link would be sent
            // out of screen.
            _menu._flickable.contentY =
                Math.max(_menu._flickableContentYAtOpen,
                         hook.y + hookHeight + Theme.paddingSmall - _menu._flickable.height)
        }
    }
    Connections {
        target: _menu && _menu._flickable ? _menu._flickable : null
        onContentHeightChanged: {
            // Update the initial opening position with the zoom factor
            // if the contentHeight is changed while menu was displayed.
            _menu._flickableContentYAtOpen *= _menu._flickable.contentHeight / _flickableContentHeight
            _flickableContentHeight = _menu._flickable.contentHeight
        }
    }

    width: parent.width
    height: hookHeight + (_menu ? Theme.paddingSmall + _menu.height : 0.)

    Rectangle {
        id: background
        parent: _menu ? _menu : null
        anchors.fill: parent ? parent : undefined
        color: Theme.highlightDimmerColor
        opacity: 0.91
        z: -1
    }
}
