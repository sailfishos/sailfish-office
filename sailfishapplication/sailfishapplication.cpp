
#include <QApplication>
#include <QDir>
#include <QGraphicsObject>


#include <QDeclarativeComponent>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeView>

#include <MDeclarativeCache>

#include "sailfishapplication.h"
#include "config.h"
#include "models/documentlistmodel.h"
#include "models/trackerdocumentprovider.h"
#include "models/documentproviderplugin.h"
#include "models/documentproviderlistmodel.h"

#include "pdf/pdfdocument.h"
#include "pdf/pdfpagemodel.h"
#include "pdf/pdfpage.h"

QSharedPointer<QApplication> Sailfish::createApplication(int &argc, char **argv)
{
    return QSharedPointer<QApplication>(MDeclarativeCache::qApplication(argc, argv));
}
    
QSharedPointer<QDeclarativeView> Sailfish::createView(const QString &file)
{
    qmlRegisterType< DocumentListModel >( "Sailfish.Office", 1, 0, "DocumentListModel" );
    qmlRegisterType< DocumentProviderListModel >( "Sailfish.Office", 1, 0, "DocumentProviderListModel" );
    qmlRegisterType< TrackerDocumentProvider >( "Sailfish.Office", 1, 0, "TrackerDocumentProvider" );
    qmlRegisterInterface< DocumentProviderPlugin >( "DocumentProviderPlugin" );

    qmlRegisterType< PDFDocument >( "Sailfish.Office.PDF", 1, 0, "Document" );
    qmlRegisterType< PDFPageModel >( "Sailfish.Office.PDF", 1, 0, "PageModel" );
    qmlRegisterType< PDFPage >( "Sailfish.Office.PDF", 1, 0, "Page" );

    QSharedPointer<QDeclarativeView> view(MDeclarativeCache::qDeclarativeView());
    view->engine()->addImportPath(CALLIGRA_QML_PLUGIN_DIR);
    view->setSource(QUrl::fromLocalFile(QML_INSTALL_DIR + file));
    return view;
}

void Sailfish::showView(const QSharedPointer<QDeclarativeView> &view) 
{
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view->setAttribute(Qt::WA_OpaquePaintEvent);
    view->setAttribute(Qt::WA_NoSystemBackground);
    view->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    view->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    view->showFullScreen();
}


