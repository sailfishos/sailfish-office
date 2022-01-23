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

#include "sailfishofficedjvuplugin.h"

#include "djvu.h"
#include "djvudocument.h"
#include "djvucanvas.h"
//#include "djvulinkarea.h"
//#include "djvuselection.h"
//#include "djvuannotation.h"

SailfishOfficeDJVUPlugin::SailfishOfficeDJVUPlugin(QObject *parent)
    : QQmlExtensionPlugin(parent)
{
}

void SailfishOfficeDJVUPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.Office.DJVU"));
    qmlRegisterType<DJVUDocument>(uri, 1, 0, "Document");
    qmlRegisterType<DJVUCanvas>(uri, 1, 0, "Canvas");
    //qmlRegisterType<DJVULinkArea>(uri, 1, 0, "LinkArea");
    //qmlRegisterType<DJVUSelection>(uri, 1, 0, "Selection");
    //qmlRegisterType<DJVUAnnotation>(uri, 1, 0, "Annotation");
    //qmlRegisterType<DJVUTextAnnotation>(uri, 1, 0, "TextAnnotation");
//    qmlRegisterType<DJVUCaretAnnotation>(uri, 1, 0, "CaretAnnotation");
//    qmlRegisterType<DJVUHighlightAnnotation>(uri, 1, 0, "HighlightAnnotation");
}
