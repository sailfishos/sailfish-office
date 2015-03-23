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
import org.kde.calligra 1.0 as Calligra

Page {
    id: page;

    property QtObject document;

    allowedOrientations: Orientation.All;

    SilicaListView {
        anchors.fill: parent;

        //: Page with Text document index
        //% "Index"
        header: PageHeader { title: qsTrId( "sailfish-office-he-index") }

        model: Calligra.ContentsModel {
            document: page.document;
        }

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
                text: model.title;
                color: (model.contentIndex == page.document.currentIndex || bg.highlighted) ? Theme.highlightColor : Theme.primaryColor;
                truncationMode: TruncationMode.Fade
            }
            Label {
                id: pageNumberLbl
                anchors {
                    right: parent.right;
                    rightMargin: Theme.horizontalPageMargin;
                    verticalCenter: parent.verticalCenter;
                }
                text: model.contentIndex + 1;
                color: (model.contentIndex == page.document.currentIndex || bg.highlighted) ? Theme.highlightColor : Theme.primaryColor;
            }

            onClicked: {
                page.document.currentIndex = model.contentIndex;
                pageStack.navigateBack(PageStackAction.Animated);
            }
        }
    }
}
