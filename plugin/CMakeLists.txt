set(plugin_SRCS
    linkarea.cpp
    sailfishofficeplugin.cpp
)

automoc4_add_library(sailfishofficeplugin MODULE ${plugin_SRCS})
target_link_libraries(sailfishofficeplugin stdc++ ${QT_LIBRARIES})

install(TARGETS sailfishofficeplugin DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/qt4/imports/Sailfish/Office)

install(FILES
    qmldir
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
