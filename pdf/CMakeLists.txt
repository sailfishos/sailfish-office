find_package(Poppler REQUIRED)

include_directories(${POPPLER_INCLUDE_DIR} ${POPPLER_QT4_INCLUDE_DIR})

set(pdfplugin_SRCS
    sailfishofficepdfplugin.cpp
    pdfdocument.cpp
    pdfpagemodel.cpp
    pdfpage.cpp
    pdfrenderthread.cpp
    pdfjob.cpp
    pdftocmodel.cpp
    pdfcanvas.cpp
)

automoc4_add_library(sailfishofficepdfplugin MODULE ${pdfplugin_SRCS})
target_link_libraries(sailfishofficepdfplugin stdc++ ${QT_LIBRARIES} ${POPPLER_LIBRARY} ${POPPLER_QT4_LIBRARY})

install(TARGETS sailfishofficepdfplugin DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/qt4/imports/Sailfish/Office/PDF)
install(FILES qmldir DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/qt4/imports/Sailfish/Office/PDF)
