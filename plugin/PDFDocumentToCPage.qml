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

Page {
    id: page;
    signal pageSelected(int pageNumber);

    property alias tocModel: tocListView.model;

    allowedOrientations: Orientation.All;

    SilicaListView {
        id: tocListView
        anchors.fill: parent;

        //: Page with PDF index
        //% "Index"
        header: PageHeader { title: qsTrId( "sailfish-office-he-pdf_index" ); }

        ViewPlaceholder {
            id: placeholder
            //% "Document has no table of content"
            text: qsTrId("sailfish-office-me-no-toc")
        }
        // The enabled attribute of the placeholder is not set by binding since
        // the model object comes from a different thread and QML cannot listen
        // on signals from a different thread. Thus, the attribute is set by
        // reading the count value instead of a binding.
        onModelChanged: placeholder.enabled = !model || (model.count == 0)

        delegate: BackgroundItem {
            id: bg;

            Label {
                anchors {
                    left: parent.left;
                    leftMargin: Theme.horizontalPageMargin + (Theme.paddingLarge * model.level);
                    right: pageNumberLbl.left;
                    rightMargin: Theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                elide: Text.ElideRight;
                text: (model.title === undefined) ? "" : model.title;
                color: bg.highlighted ? Theme.highlightColor : Theme.primaryColor;
                truncationMode: TruncationMode.Fade;
            }
            Label {
                id: pageNumberLbl
                anchors {
                    right: parent.right;
                    rightMargin: Theme.paddingLarge;
                    verticalCenter: parent.verticalCenter;
                }
                text: (model.pageNumber === undefined) ? "" : model.pageNumber;
                color: bg.highlighted ? Theme.highlightColor : Theme.primaryColor;
            }

            onClicked: {
                page.pageSelected(model.pageNumber - 1);
                pageStack.navigateBack(PageStackAction.Animated);
            }
        }
    }
}
