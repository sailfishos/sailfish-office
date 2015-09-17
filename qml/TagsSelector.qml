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

Page {
    id: page

    property bool editing
    property alias title: header.title
    property alias model: tags.model //selectionModel.sourceModel
    property var highlight // An object with three methods :
                           // hasTag(tag), addTag(tag), removeTag(tag)

    /*SortFilterSelectionModel {
        id: selectionModel

        filter {
            property: "label"
            value: searchField.text
        }
        sort {
            property: "label"
            order: Qt.AscendingOrder
        }
    }*/

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

            Flow {
                id: selection

                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                //height: parent.height - searchField.height - column.spacing
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

                        onClicked: if (selected) {
                            selected = false
                            page.highlight.removeTag(model.label)
                        } else {
                            selected = true
                            page.highlight.addTag(model.label)
                        }
                    }
                    //model: selectionModel
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
