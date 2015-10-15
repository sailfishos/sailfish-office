/*
 * Copyright (C) 2015 François Kubler and Caliste Damien.
 * Contact: François Kubler <francois@kubler.org>
 *          Damien Caliste <dcaliste@free.fr>
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
import Sailfish.Office.Files 1.0

Page {
    id: page

    property bool editing
    property alias title: header.title
    property alias model: selectionModel.sourceModel
    property var highlight // An object with three methods :
                           // hasTag(tag), addTag(tag), removeTag(tag)

    TagFilterModel {
        id: selectionModel
        filterRegExp: RegExp(searchField.text, "i")
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            
            anchors {
                left: parent.left
                right: parent.right
            }

            PageHeader {
                id: header
            }

            SearchField {
                id: searchField

                anchors {
                    left: parent.left
                    right: parent.right
                }

                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: focus = false
            }

            InfoLabel {
                id: placeholder
                //% "No tag exists."
                text: qsTrId("sailfish-office-no-tag")
                visible: opacity > 0
                opacity: searchField.text === "" && page.model.count == 0 ? 1.0 : 0.0
                Behavior on opacity { FadeAnimation {} }
            }
            InfoLabel {
                font.pixelSize: Theme.fontSizeLarge
                color: Theme.rgba(Theme.highlightColor, 0.4)
                //% "Create one by typing in the above search field."
                text: qsTrId("sailfish-office-no-tag-hint")
                visible: opacity > 0
                opacity: placeholder.opacity
            }

            Flow {
                id: selection

                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                spacing: Theme.paddingMedium

                MouseArea {
                    width: newItem.width + icon.width
                    height: Math.max(newItem.height, icon.height)
                    // This item always visible unless :
                    //     - searchField is empty ;
                    //     - OR there is already a tag by that exact name.
                    visible: page.editing && !(searchField.text === "" ||
                                               page.model.contains(searchField.text))
                    Tag {
                        id: newItem
                        anchors {
                            verticalCenter: parent.verticalCenter
                        }

                        tag: searchField.text.trim()
                        selected: false
                    }
                    Image {
                        id: icon
                        anchors {
                            left: newItem.right
                            verticalCenter: parent.verticalCenter
                        }
                        source: "image://theme/icon-m-add"
                    }

                    onClicked: {
                        page.highlight.addTag(newItem.tag)
                        searchField.text = ""
                    }
                }

                Repeater {
                    id: tags
                    delegate: Tag {
                        id: tagDelegate

                        selected: page.highlight.hasTag(model.label)
                        tag: model.label

                        onClicked: selected = !selected
                        onSelectedChanged: if (selected) {
                            page.highlight.addTag(model.label)
                        } else {
                            page.highlight.removeTag(model.label)
                        }
                    }
                    model: selectionModel
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
