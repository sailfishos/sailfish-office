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
import Sailfish.Office 1.0

Page {
    id: page

    property url source
    property int indexCount
    property string mimeType

    FileInfo {
        id: info
        source: page.source
    }

    SilicaFlickable {
        id: flickable
        anchors.fill: parent
        contentHeight: contentColumn.height + Theme.paddingLarge

        Column {
            id: contentColumn
            width: parent.width

            PageHeader {
                id: detailsHeader
                //: Details page title
                //% "Details"
                title: qsTrId("sailfish-office-he-details")
            }

            DetailItem {
                //: File name detail of the document
                //% "File Name"
                label: qsTrId("sailfish-office-la-filename")
                value: info.fileName
            }

            DetailItem {
                //: File size detail of the document
                //% "Size"
                label: qsTrId("sailfish-office-la-filesize")
                value: Format.formatFileSize(info.fileSize)
            }

            DetailItem {
                //: File type detail of the document
                //% "Type"
                label: qsTrId("sailfish-office-la-filetype")
                value: info.mimeTypeComment
            }

            DetailItem {
                //: Last modified date of the document
                //% "Last Modified"
                label: qsTrId("sailfish-office-la-lastmodified")
                value: Format.formatDate(info.modifiedDate, Format.DateFull)
            }

            DetailItem {
                label: {
                    switch(page.mimeType) {
                        case "application/vnd.oasis.opendocument.spreadsheet":
                        case "application/x-kspread":
                        case "application/vnd.ms-excel":
                        case "text/csv":
                        case "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet":
                        case "application/vnd.openxmlformats-officedocument.spreadsheetml.template":
                            //: Sheet count of the spreadsheet
                            //% "Sheets"
                            return qsTrId("sailfish-office-la-sheetcount")
                        case "application/vnd.oasis.opendocument.presentation":
                        case "application/vnd.oasis.opendocument.presentation-template":
                        case "application/x-kpresenter":
                        case "application/vnd.ms-powerpoint":
                        case "application/vnd.openxmlformats-officedocument.presentationml.presentation":
                        case "application/vnd.openxmlformats-officedocument.presentationml.template":
                            //: Slide count detail of the presentation
                            //% "Slides"
                            return qsTrId("sailfish-office-la-slidecount")
                        case "application/vnd.oasis.opendocument.text-master":
                        case "application/vnd.oasis.opendocument.text":
                        case "application/vnd.oasis.opendocument.text-template":
                        case "application/msword":
                        case "application/rtf":
                        case "application/x-mswrite":
                        case "application/vnd.openxmlformats-officedocument.wordprocessingml.document":
                        case "application/vnd.openxmlformats-officedocument.wordprocessingml.template":
                        case "application/vnd.ms-works":
                            //: Page count of the text document
                            //% "Page Count"
                            return qsTrId("sailfish-office-la-pagecount")
                        case "application/pdf":
                            //: Page count of the text document
                            //% "Page Count"
                            return qsTrId("sailfish-office-la-pagecount")
                        default:
                            //: Index count for unknown document types.
                            //% "Index Count"
                            return qsTrId("sailfish-office-la-indexcount")
                    }
                }
                value: page.indexCount
            }
        }

        VerticalScrollDecorator {}
    }
}
