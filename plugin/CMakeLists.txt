set(plugin_SRCS
    fileinfo.cpp
    sailfishofficeplugin.cpp
)

add_library(sailfishofficeplugin MODULE ${plugin_SRCS})
qt5_use_modules(sailfishofficeplugin Widgets Quick)
target_link_libraries(sailfishofficeplugin stdc++ ${QT_LIBRARIES})

install(TARGETS sailfishofficeplugin DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/qt5/qml/Sailfish/Office)

install(FILES
    qmldir
    ContextMenuHook.qml
    DocumentPage.qml
    DocumentsSharingList.qml
    PDFDocumentPage.qml
    PDFDocumentToCPage.qml
    PDFView.qml
    PDFSelectionView.qml
    PDFSelectionHandle.qml
    PDFSelectionDrag.qml
    PDFStorage.js
    PresentationPage.qml
    PresentationThumbnailPage.qml
    SpreadsheetListPage.qml
    SpreadsheetPage.qml
    TextDocumentPage.qml
    TextDocumentToCPage.qml
    ZoomableThumbnail.qml
    SearchBarItem.qml
    ToolBar.qml
    DetailsPage.qml
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/qt5/qml/Sailfish/Office
)
