set(plugin_SRCS
    sailfishofficeplugin.cpp
)

add_library(sailfishofficeplugin MODULE ${plugin_SRCS})
qt5_use_modules(sailfishofficeplugin Widgets Quick)
target_link_libraries(sailfishofficeplugin stdc++ ${QT_LIBRARIES})

install(TARGETS sailfishofficeplugin DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/qt4/imports/Sailfish/Office)

install(FILES
    qmldir
    DocumentPage.qml
    DocumentsSharingList.qml
    PDFDocumentPage.qml
    PDFDocumentToCPage.qml
    PresentationPage.qml
    PresentationThumbnailPage.qml
    SpreadsheetListPage.qml
    SpreadsheetPage.qml
    TextDocumentPage.qml
    TextDocumentToCPage.qml
    ZoomableThumbnail.qml
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/qt4/imports/Sailfish/Office
)
