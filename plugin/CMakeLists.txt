find_package(PkgConfig REQUIRED)
pkg_check_modules(SAILFISHSILICA sailfishsilica REQUIRED)
pkg_check_modules(ICU icu-i18n REQUIRED)

include_directories(
    ${SAILFISHSILICA_INCLUDE_DIRS}
    ${ICU_INCLUDE_DIRS}
)

set(plugin_SRCS
    plaintextmodel.cpp
    sailfishofficeplugin.cpp
)

add_library(sailfishofficeplugin MODULE ${plugin_SRCS})
qt5_use_modules(sailfishofficeplugin Widgets Quick)
target_link_libraries(sailfishofficeplugin stdc++ ${QT_LIBRARIES} ${SAILFISHSILICA_LIBRARIES} ${ICU_LIBRARIES})

install(TARGETS sailfishofficeplugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/qml/Sailfish/Office)

install(FILES
    qmldir
    CalligraDocumentPage.qml
    ContextMenuHook.qml
    ControllerFlickable.qml
    DeleteButton.qml
    DetailsPage.qml
    DJVUAnnotationEdit.qml
    DJVUAnnotationNew.qml
    DJVUContextMenuHighlight.qml
    DJVUContextMenuLinks.qml
    DJVUContextMenuText.qml
    DJVUDocumentPage.qml
    DJVUDocumentToCPage.qml
    DJVUSelectionDrag.qml
    DJVUSelectionHandle.qml
    DJVUSelectionView.qml
    DJVUStorage.js
    DJVUView.qml
    DocumentFlickable.qml
    DocumentHeader.qml
    DocumentPage.qml
    FullscreenError.qml
    IndexButton.qml
    OverlayToolbar.qml
    PDFAnnotationEdit.qml
    PDFAnnotationNew.qml
    PDFContextMenuHighlight.qml
    PDFContextMenuLinks.qml
    PDFContextMenuText.qml
    PDFDocumentPage.qml
    PDFDocumentToCPage.qml
    PDFSelectionDrag.qml
    PDFSelectionHandle.qml
    PDFSelectionView.qml
    PDFStorage.js
    PDFView.qml
    PlainTextDocumentPage.qml
    PresentationPage.qml
    PresentationThumbnailPage.qml
    SearchBarItem.qml
    ShareButton.qml
    SpreadsheetListPage.qml
    SpreadsheetPage.qml
    TextDocumentPage.qml
    TextDocumentToCPage.qml
    ToolBar.qml
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/qml/Sailfish/Office
)
