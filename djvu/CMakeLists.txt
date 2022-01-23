find_package(PkgConfig REQUIRED)
find_package(Qt5Xml REQUIRED)
pkg_check_modules(SAILFISHSILICA sailfishsilica REQUIRED)
pkg_check_modules(DJVULIBRE ddjvuapi REQUIRED)

include_directories(
    ${QT5XML_INCLUDE_DIR}
    ${DJVULIBRE_INCLUDE_DIR}
    )

set(djvuplugin_SRCS
    sailfishofficedjvuplugin.cpp
    djvu.cpp
    djvudocument.cpp
    djvurenderthread.cpp
    djvujob.cpp
    djvutocmodel.cpp
    djvucanvas.cpp
    #djvulinkarea.cpp
    #djvusearchmodel.cpp
    #djvuselection.cpp
    #djvuannotation.cpp
)

add_library(sailfishofficedjvuplugin MODULE ${djvuplugin_SRCS})
qt5_use_modules(sailfishofficedjvuplugin Quick)
target_link_libraries(sailfishofficedjvuplugin stdc++ ${QT_LIBRARIES} ${QT5XML_LIBRARIES} ${DJVULIBRE_LIBRARIES})

install(TARGETS sailfishofficedjvuplugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/qml/Sailfish/Office/DJVU)
install(FILES qmldir DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/qml/Sailfish/Office/DJVU)
