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
    id: contextMenuHighlight
    /*
    property Annotation annotation

    InfoLabel {
        id: infoContents
        visible: infoContents.text != ""
        width: parent.width
        height: implicitHeight + 2 * Theme.paddingSmall
        font.pixelSize: Theme.fontSizeSmall
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        maximumLineCount: 2
        color: Theme.highlightColor
        opacity: .6
        text: {
            if (contextMenuHighlight.annotation
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
            model: [{"style": HighlightAnnotation.Highlight,
                     "label": "abc"},
                    {"style": HighlightAnnotation.Squiggly,
                     "label": "a̰b̰c̰"},
                    {"style": HighlightAnnotation.Underline,
                     "label": "<span style=\"text-decoration:underline\">abc</span>"},
                    {"style": HighlightAnnotation.StrikeOut,
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
                           || (contextMenuHighlight.annotation
                               && contextMenuHighlight.annotation.style == modelData["style"])
                           ? Theme.highlightColor : Theme.primaryColor
                    Rectangle {
                        visible: modelData["style"] == HighlightAnnotation.Highlight
                        anchors.fill: parent
                        color: bgStyle.highlighted ? Theme.highlightColor : Theme.primaryColor
                        opacity: Theme.opacityLow
                        z: -1
                    }
                }
            }
        }
    }
    MenuItem {
        visible: contextMenuHighlight.annotation
        text: contextMenuHighlight.annotation
              && contextMenuHighlight.annotation.contents == ""
              //% "Add a comment"
              ? qsTrId("sailfish-office-me-djvu-hl-anno-comment")
              //% "Edit the comment"
              : qsTrId("sailfish-office-me-djvu-hl-anno-comment-edit")
        onClicked: {
            if (contextMenuHighlight.annotation.contents == "") {
                doc.create(contextMenuHighlight.annotation)
            } else {
                doc.edit(contextMenuHighlight.annotation)
            }
        }
    }
    MenuItem {
        //% "Clear"
        text: qsTrId("sailfish-office-me-djvu-hl-anno-clear")
        onClicked: contextMenuHighlight.annotation.remove()
    }*/
}
