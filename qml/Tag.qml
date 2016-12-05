/*
 * Copyright (C) 2015 François Kubler.
 * Contact: François Kubler <francois@kubler.org>
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
  

/**
 * The default Tag is a Label with a transparent background and a Theme.primaryColor color.
 *
 * When enabled, a Tag can be selected or deselected.
 *
 * If enabled and selected :
 *     The background is shown,
 *     The label color is Theme.highlightColor.
 *
 * If not enabled and selected : (typically, when you just want to display a list of Tags)
 *     The background is shown,
 *     The label color is Theme.primaryColor.
 *
 * If unselected (regardless of enabled) :
 *     The background is transparent,
 *     The label color is Theme.primaryColor.
 *
 */

MouseArea {
    id: root
    

    property alias color: rect.color
    property alias fontColor: label.color
    property alias fontSize: label.font.pixelSize
    property alias tag: label.text
    

    property bool highlighted: root.pressed && root.containsMouse
    property bool selected: false
    

    

    height: label.height + (enabled ? Theme.paddingSmall : 0) * 2
    width: label.width + (enabled ? Theme.paddingLarge : Theme.paddingSmall) * 2
    

    

    Rectangle {
        id: rect
        

        anchors {
            centerIn: parent
            fill: parent
        }
        color: "transparent"
        radius: 9
        

        Label {
            id: label
            

            anchors {
                centerIn: parent
            }
            color: root.enabled ? root.highlighted ? Theme.highlightColor
                                                   : Theme.primaryColor
                                : Theme.highlightColor
            font {
                //capitalization: Font.AllLowercase
                  pixelSize: Theme.fontSizeMedium
            }
        }
    }
    

    ListView.onAdd: AddAnimation {
        target: root
    }
    

    ListView.onRemove: RemoveAnimation {
        target: root
    }
    

    states: [
        State {
            name: "SELECTED"
            when: root.selected
            

            PropertyChanges {
                color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
                target: rect
            }
        },
        

        State {
            name: "NOT_SELECTED"
            when: !root.selected
            

            PropertyChanges {
                color: "transparent"
                target: rect
            }
        }
    ]
    

    transitions: [
        Transition {
            reversible: true
            to: "*"
            

            ColorAnimation {
                duration: 100
                easing.type: Easing.InOutQuad
                target: rect
            }
        }
    ]
}