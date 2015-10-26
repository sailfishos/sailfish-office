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

DocumentPage {
    id: page

    attachedPage: Component {
        TextDocumentToCPage {
            document: doc
        }
    }

    Calligra.View {
        id: documentView

        width: page.width
        height: page.height

        document: doc
    }

    SilicaFlickable {
        id: documentFlickable

        width: page.width
        height: page.height

        Calligra.ViewController {
            id: controller
            view: documentView
            flickable: documentFlickable
            useZoomProxy: false
            maximumZoom: 5.0
            minimumZoomFitsWidth: true
        }

        children: [
            HorizontalScrollDecorator { color: Theme.highlightDimmerColor },
            VerticalScrollDecorator { color: Theme.highlightDimmerColor }
        ]

        PinchArea {
            anchors.fill: parent

            onPinchUpdated: {
                var newCenter = mapToItem(documentFlickable, pinch.center.x, pinch.center.y)
                controller.zoomAroundPoint(controller.zoom * (pinch.scale - pinch.previousScale), newCenter.x, newCenter.y)
            }
            onPinchFinished: controller.zoomTimeout()

            Calligra.LinkArea {
                anchors.fill: parent
                document: doc
                onClicked: page.open = !page.open
                onLinkClicked: Qt.openUrlExternally(linkTarget)
                controllerZoom: controller.zoom
            }
        }
    }

    Calligra.Document {
        id: doc

        onStatusChanged: if (status == Calligra.DocumentStatus.Loaded) controller.zoomToFitWidth(page.width)
    }
    
    busy: doc.status != Calligra.DocumentStatus.Loaded
    source: doc.source
    indexCount: doc.indexCount

    onStatusChanged: {
        //Delay loading the document until the page has been activated.
        if (status == PageStatus.Active) {
            doc.source = page.path
        }
    }
}
