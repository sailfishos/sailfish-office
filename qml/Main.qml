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
import Sailfish.Office.Files 1.0

ApplicationWindow {
    id: window

    property Item documentItem
    property QtObject fileListModel: trackerProvider.model
    property Page _mainPage

    allowedOrientations: defaultAllowedOrientations
    _defaultLabelFormat: Text.PlainText
    _defaultPageOrientations: Orientation.All
    cover: Qt.resolvedUrl("CoverPage.qml")
    initialPage: Component {
        FileListPage {
            id: fileListPage

            model: trackerProvider.model
            provider: trackerProvider
            Component.onCompleted: window._mainPage = fileListPage
        }
    }

    /* Currently tracker is the only source for documents, so DocumentProviderListModel is unused
    DocumentProviderListModel {
        id: documentProviderListModel

    }*/
    TrackerDocumentProvider {
        id: trackerProvider
    }

    FileInfo {
        id: fileInfo
    }

    function openFile(file) {
        fileInfo.source = file

        pageStack.pop(window._mainPage, PageStackAction.Immediate)

        var handler = ""

        switch (fileInfo.mimeType) {
        case "application/vnd.oasis.opendocument.spreadsheet":
        case "application/x-kspread":
        case "application/vnd.ms-excel":
        case "text/csv":
        case "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet":
        case "application/vnd.openxmlformats-officedocument.spreadsheetml.template":
            handler = "Sailfish.Office.SpreadsheetPage"
            break

        case "application/vnd.oasis.opendocument.presentation":
        case "application/vnd.oasis.opendocument.presentation-template":
        case "application/x-kpresenter":
        case "application/vnd.ms-powerpoint":
        case "application/vnd.openxmlformats-officedocument.presentationml.presentation":
        case "application/vnd.openxmlformats-officedocument.presentationml.template":
            handler = "Sailfish.Office.PresentationPage"
            break

        case "application/vnd.oasis.opendocument.text-master":
        case "application/vnd.oasis.opendocument.text":
        case "application/vnd.oasis.opendocument.text-template":
        case "application/msword":
        case "application/rtf":
        case "application/x-mswrite":
        case "application/vnd.openxmlformats-officedocument.wordprocessingml.document":
        case "application/vnd.openxmlformats-officedocument.wordprocessingml.template":
        case "application/vnd.ms-works":
            handler = "Sailfish.Office.TextDocumentPage"
            break

        case "application/pdf":
            handler = "Sailfish.Office.PDFDocumentPage"
            break

        default:
            console.log("Warning: Unrecognised file type for file " + fileInfo.fullPath)
        }

        if (handler != "") {
            pageStack.push(handler,
                           { title: fileInfo.fileName, path: fileInfo.fullPath, mimeType: fileInfo.mimeType },
                           PageStackAction.Immediate)
        }

        activate()
    }

    function mimeToIcon(fileMimeType) {
        var iconType = "other"
        switch (fileMimeType) {
        case "text/plain":
        case "text/x-vnote":
            iconType = "note"
            break
        case "application/pdf":
            iconType = "pdf"
            break
        case "application/vnd.oasis.opendocument.spreadsheet":
        case "application/x-kspread":
        case "application/vnd.ms-excel":
        case "text/csv":
        case "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet":
        case "application/vnd.openxmlformats-officedocument.spreadsheetml.template":
            iconType = "spreadsheet"
            break
        case "application/vnd.oasis.opendocument.presentation":
        case "application/vnd.oasis.opendocument.presentation-template":
        case "application/x-kpresenter":
        case "application/vnd.ms-powerpoint":
        case "application/vnd.openxmlformats-officedocument.presentationml.presentation":
        case "application/vnd.openxmlformats-officedocument.presentationml.template":
            iconType = "presentation"
            break
        case "application/vnd.oasis.opendocument.text-master":
        case "application/vnd.oasis.opendocument.text":
        case "application/vnd.oasis.opendocument.text-template":
        case "application/msword":
        case "application/rtf":
        case "application/x-mswrite":
        case "application/vnd.openxmlformats-officedocument.wordprocessingml.document":
        case "application/vnd.openxmlformats-officedocument.wordprocessingml.template":
        case "application/vnd.ms-works":
            iconType = "formatted"
            break
        }
        return "image://theme/icon-m-file-" + iconType
    }
}
