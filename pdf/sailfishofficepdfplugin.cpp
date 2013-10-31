/*
 *
 */

#include "sailfishofficepdfplugin.h"

#include "pdfdocument.h"
#include "pdfpagemodel.h"
#include "pdfpage.h"
#include "pdfcanvas.h"

SailfishOfficePDFPlugin::SailfishOfficePDFPlugin( QObject* parent )
    : QQmlExtensionPlugin( parent )
{

}

void SailfishOfficePDFPlugin::registerTypes(const char* uri)
{
    Q_ASSERT( uri == QLatin1String( "Sailfish.Office.PDF" ) );
    qmlRegisterType< PDFDocument >( uri, 1, 0, "Document" );
    qmlRegisterType< PDFPageModel >( uri, 1, 0, "PageModel" );
    qmlRegisterType< PDFPage >( uri, 1, 0, "Page" );
    qmlRegisterType< PDFCanvas >( uri, 1, 0, "Canvas" );
}

//Q_EXPORT_PLUGIN2(sailfishofficepdfplugin, SailfishOfficePDFPlugin)
