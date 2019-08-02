/*
 * Copyright (C) 2019 Jolla Ltd.
 * Contact: Pekka Vuorela <pekka.vuorela@jolla.com>
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

import QtQuick 2.6
import Sailfish.Silica 1.0

TouchBlocker {
    id: root

    property string error
    property string localizedError: {
        // Match hard-coded error string from calligra / KoDocument.cpp
        // Ideally there would be Calligra localization available, but it
        // a) is stored in separate kde subversion repository together with all kinds of kde things
        // b) weights some 2-3MB per language
        // So since this is should be the only place really showing Calligra strings, let's just
        // hack a separate translation for the few known cases. Likely even out of these many won't be
        // ever shown to the user.
        var re = new RegExp("Could not open file://(.*)\\.\\nReason: (.*)\\.\\n(.*)")
        var matches = re.exec(error)
        if (matches && matches.length == 4) {
            //% "Could not open file:"
            return qsTrId("sailfish-calligra_open_error") + "\n" + matches[1]
                    + "\n\n" + localizeOpenError(matches[2])
        } else {
            console.log("Unable to parse Calligra error string", error)
            return error
        }
    }

    anchors.fill: parent

    function localizeOpenError(error) {
        switch(error) {
        case "Could not create the filter plugin":
            //% "Could not create the filter plugin"
            return qsTrId("office_calligra_error-could_not_create_filter_plugin")
        case "Could not create the output document":
            //% "Could not create the output document"
            return qsTrId("office_calligra_error-could_not_create_output_document")
        case "File not found":
            //% "File not found"
            return qsTrId("office_calligra_error-file_not_found")
        case "Cannot create storage":
            //% "Cannot create storage"
            return qsTrId("office_calligra_error-cannot_create_storage")
        case "Bad MIME type":
            //% "Bad MIME type"
            return qsTrId("office_calligra_error-bad_mime_type")
        case "Error in embedded document":
            //% "Error in embedded document"
            return qsTrId("office_calligra_error-error_in_embedded_document")
        case "Format not recognized":
            //% "Format not recognized"
            return qsTrId("office_calligra_error-format_not_recognized")
        case "Not implemented":
            //% "Not implemented"
            return qsTrId("office_calligra_error-not_implemented")
        case "Parsing error":
            //% "Parsing error"
            return qsTrId("office_calligra_error-parsing_error")
        case "Document is password protected":
            //% "Document is password protected"
            return qsTrId("office_calligra_error-password_protected_file")
        case "Invalid file format":
            //% "Invalid file format"
            return qsTrId("office_calligra_error-invalid_file_format")
        case "Internal error":
            //% "Internal error"
            return qsTrId("office_calligra_error-internal_error")
        case "Out of memory":
            //% "Out of memory"
            return qsTrId("office_calligra_error-out_of_memory")
        case "Empty Filter Plugin":
            //% "Empty Filter Plugin"
            return qsTrId("office_calligra_error-empty_filter_plugin")
        case "Trying to load into the wrong kind of document":
            //% "Trying to load into the wrong kind of document"
            return qsTrId("office_calligra_error-wrong_kind_of_document")
        case "Failed to download remote file":
            //% "Failed to download remote file"
            return qsTrId("office_calligra_error-faile_to_download_remote_file")
        case "Unknown error":
            //% "Unknown error"
            return qsTrId("office_calligra_error-unknown")
        }

        return error
    }

    Rectangle {
        anchors.fill: parent
        opacity: Theme.opacityLow
        color: Theme.highlightDimmerColor
    }

    Column {
        x: Theme.horizontalPageMargin
        width: parent.width - 2*x
        spacing: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter

        HighlightImage {
            id: warningIcon

            anchors.horizontalCenter: parent.horizontalCenter
            source: "image://theme/icon-l-attention"
            highlighted: true
        }

        Label {
            width: parent.width
            text: localizedError
            wrapMode: Text.Wrap
            color: Theme.highlightColor
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
