/*
 *
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
