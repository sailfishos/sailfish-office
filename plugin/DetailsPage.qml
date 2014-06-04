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
import org.kde.calligra 1.0 as Calligra

Page {
    id: page;

    property url source;
    property int indexCount;

    FileInfo {
        id: info;
        source: page.source;
    }

    Column {
        width: parent.width
        PageHeader {
            //: Details page title
            //% "Details"
            title: qsTrId("sailfish-office-he-details");
        }

        DetailsItem {
            //: File name detail of the document
            //% "File Name"
            detail: qsTrId("sailfish-office-la-filename");
            value: info.fileName;
        }

        DetailsItem {
            //: File size detail of the document
            //% "Size"
            detail: qsTrId("sailfish-office-la-filesize");
            value: Format.formatFileSize(info.fileSize);
        }

        DetailsItem {
            //: File type detail of the document
            //% "Type"
            detail: qsTrId("sailfish-office-la-filetype");
            value: info.mimeTypeComment;
        }

        DetailsItem {
            //: Last modified date of the document
            //% "Last Modified"
            detail: qsTrId("sailfish-office-la-lastmodified");
            value: Format.formatDate(info.modifiedDate, Format.DateFull);
        }

        DetailsItem {
            detail: {
                switch(Calligra.Global.documentType(page.source)) {
                    case Calligra.DocumentType.TextDocument:
                        //: Page count of the text document
                        //% "Page Count"
                        return qsTrId("sailfish-office-la-pagecount");
                    case Calligra.DocumentType.Spreadsheet:
                        //: Sheet count of the spreadsheet
                        //% "Sheets"
                        return qsTrId("sailfish-office-la-sheetcount");
                    case Calligra.DocumentType.Presentation:
                        //: Slide count detail of the presentation
                        //% "Slides"
                        return qsTrId("sailfish-office-la-slidecount");
                    case Calligra.DocumentType.StaticTextDocument:
                        //: Page count of the text document
                        //% "Page Count"
                        return qsTrId("sailfish-office-la-pagecount");
                    default:
                        //: Index count for unknown document types.
                        //% "Index Count"
                        return qsTrId("sailfish-office-la-indexcount");
                }
            }
            value: page.indexCount;
        }
    }
}
