/*
 * Copyright (C) 2016 Caliste Damien.
 * Copyright (C) 2022 Yura Beznos <yura.beznos@you-ra.info>
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
import Sailfish.Office.DJVU 1.0

ContextMenu {
    id: contextMenuText
    /*
    property Annotation annotation
    property point at
    
    MenuItem {
        visible: !contextMenuText.annotation
        //% "Add note"
        text: qsTrId("sailfish-office-me-djvu-txt-anno-add")
        onClicked: {
            var annotation = textComponent.createObject(contextMenuText)
            annotation.color = "#202020"
            doc.create(annotation,
                               function() {
                                   var at = view.getPositionAt(contextMenuText.at)
                                   annotation.attachAt(doc, at[0], at[2], at[1])
                               })
        }
        Component {
            id: textComponent
            TextAnnotation { }
        }
    }
    MenuItem {
        visible: contextMenuText.annotation
        //% "Edit"
        text: qsTrId("sailfish-office-me-djvu-txt-anno-edit")
        onClicked: doc.edit(contextMenuText.annotation)
    }
    MenuItem {
        visible: contextMenuText.annotation
        //% "Delete"
        text: qsTrId("sailfish-office-me-djvu-txt-anno-clear")
        onClicked: contextMenuText.annotation.remove()
    }*/
}
