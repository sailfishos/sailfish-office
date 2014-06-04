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
import Sailfish.Office 1.0

//TODO: This really should be in plugin/ but also should be a bit
//cleaner so someone can simply instantiate an object and call a
//method on that to open a document.
Item {
    property alias textDocument: textDocumentPage;
    property alias spreadsheet: spreadsheetPage;
    property alias presentation: presentationPage;
    property alias pdf: pdfPage;

    Component {
        id: textDocumentPage;
        TextDocumentPage { }
    }

    Component {
        id: spreadsheetPage;
        SpreadsheetPage { }
    }

    Component {
        id: presentationPage;
        PresentationPage { }
    }

    Component {
        id: pdfPage;
        PDFDocumentPage { }
    }
}
