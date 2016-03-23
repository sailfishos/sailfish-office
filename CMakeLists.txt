project(sailfish-office)

cmake_minimum_required(VERSION 2.8.6)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(QtSparql REQUIRED)
find_package(Booster REQUIRED)

include(cmake/QtTranslationWithID.cmake)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDES}
    ${BOOSTER_INCLUDE_DIR}
    ${QTSPARQL_INCLUDE_DIR}
)

set(CALLIGRA_QML_PLUGIN_DIR /usr/lib/calligra CACHE PATH "The location of the Calligra QtQuick Components")
configure_file(config.h.in config.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fPIC -pie -rdynamic -Wall")

add_subdirectory(plugin)
add_subdirectory(pdf)

set(sailfishoffice_SRCS
    main.cpp
    dbusadaptor.cpp
    models/filtermodel.cpp
    models/documentlistmodel.cpp
    models/documentproviderlistmodel.cpp
    models/documentprovider.cpp
    models/trackerdocumentprovider.cpp
)

set(sailfishoffice_QML_SRCS
    qml/CoverPage.qml
    qml/FileListPage.qml
    qml/Main.qml
    qml/CoverFileItem.qml
)

file(GLOB plugin_QML_SRCS plugin/*.qml)

set(sailfishoffice_TS_SRCS
    ${sailfishoffice_SRCS}
    ${sailfishoffice_QML_SRCS}
    ${plugin_QML_SRCS}
)

create_translation(engen_qm_file ${CMAKE_BINARY_DIR}/sailfish-office.ts ${sailfishoffice_TS_SRCS})

add_executable(sailfish-office ${sailfishoffice_SRCS} ${engen_qm_file})
qt5_use_modules(sailfish-office Widgets Quick DBus)
target_link_libraries(sailfish-office stdc++ ${QT_LIBRARIES} ${BOOSTER_LIBRARY} ${QTSPARQL_LIBRARY})

install(TARGETS sailfish-office DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES
    sailfish-office.desktop
    sailfish-office-openfile.desktop
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(FILES ${sailfishoffice_QML_SRCS} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sailfish-office
)
install(FILES ${engen_qm_file} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/translations)
install(FILES ${CMAKE_BINARY_DIR}/sailfish-office.ts DESTINATION ${CMAKE_INSTALL_PREFIX}/share/translations/source)
install(FILES org.sailfish.office.service DESTINATION ${CMAKE_INSTALL_PREFIX}/share/dbus-1/services)
install(FILES org.sailfish.office.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/dbus-1/interfaces)

