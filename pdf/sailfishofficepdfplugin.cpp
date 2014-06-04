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

#include "sailfishofficepdfplugin.h"

#include "pdfdocument.h"
#include "pdfcanvas.h"
#include "pdflinkarea.h"

SailfishOfficePDFPlugin::SailfishOfficePDFPlugin( QObject* parent )
    : QQmlExtensionPlugin( parent )
{

}

void SailfishOfficePDFPlugin::registerTypes(const char* uri)
{
    Q_ASSERT( uri == QLatin1String( "Sailfish.Office.PDF" ) );
    qmlRegisterType< PDFDocument >( uri, 1, 0, "Document" );
    qmlRegisterType< PDFCanvas >( uri, 1, 0, "Canvas" );
    qmlRegisterType< PDFLinkArea >( uri, 1, 0, "LinkArea" );
}

//Q_EXPORT_PLUGIN2(sailfishofficepdfplugin, SailfishOfficePDFPlugin)
