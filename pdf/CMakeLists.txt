find_package(Poppler REQUIRED)
find_package(Qt5Sql REQUIRED)

include_directories(${POPPLER_INCLUDE_DIR} ${POPPLER_QT5_INCLUDE_DIR})

set(pdfplugin_SRCS
    sailfishofficepdfplugin.cpp
    pdfdocument.cpp
    pdfrenderthread.cpp
    pdfjob.cpp
    pdftocmodel.cpp
    pdfcanvas.cpp
    pdflinkarea.cpp
    pdfsearchmodel.cpp
    pdfselection.cpp
    pdfannotation.cpp
)

add_library(sailfishofficepdfplugin MODULE ${pdfplugin_SRCS})
qt5_use_modules(sailfishofficepdfplugin Quick)
qt5_use_modules(sailfishofficepdfplugin Sql)
target_link_libraries(sailfishofficepdfplugin stdc++ ${QT_LIBRARIES} ${POPPLER_LIBRARY} ${POPPLER_QT5_LIBRARY})

install(TARGETS sailfishofficepdfplugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/qml/Sailfish/Office/PDF)
install(FILES qmldir DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/qml/Sailfish/Office/PDF)
