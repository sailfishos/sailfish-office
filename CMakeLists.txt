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
find_package(JollaSignonUiService REQUIRED)

include(cmake/QtTranslationWithID.cmake)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDES}
    ${BOOSTER_INCLUDE_DIR}
    ${QTSPARQL_INCLUDE_DIR}
    ${JOLLASIGNONUISERVICE_INCLUDE_DIR}
)

set(CALLIGRA_QML_PLUGIN_DIR /usr/lib/calligra CACHE PATH "The location of the Calligra QtQuick Components")
configure_file(config.h.in config.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fPIC -pie -rdynamic -Wall")

add_subdirectory(plugin)
#add_subdirectory(pdf)
add_subdirectory(dropbox)

set(sailfishoffice_SRCS
    main.cpp
    sailfishapplication.cpp
    models/documentlistmodel.cpp
    models/documentproviderlistmodel.cpp
    models/documentproviderplugin.cpp
    models/trackerdocumentprovider.cpp
    models/dropboxdocumentprovider.cpp
)

set(sailfishoffice_QML_SRCS
    qml/CoverPage.qml
    qml/FileListPage.qml
    qml/Main.qml
    qml/StartPage.qml
)

file(GLOB plugin_QML_SRCS plugin/*.qml)

set(sailfishoffice_TS_SRCS
    ${sailfishoffice_SRCS}
    ${sailfishoffice_QML_SRCS}
    ${plugin_QML_SRCS}
)

create_translation(engen_qm_file ${CMAKE_BINARY_DIR}/sailfish-office_eng_en.ts ${sailfishoffice_TS_SRCS})

add_executable(sailfish-office ${sailfishoffice_SRCS} ${engen_qm_file})
qt5_use_modules(sailfish-office Widgets Quick DBus)
target_link_libraries(sailfish-office stdc++ ${QT_LIBRARIES} ${BOOSTER_LIBRARY} ${QTSPARQL_LIBRARY} ${JOLLASIGNONUISERVICE_LIBRARY})

install(TARGETS sailfish-office DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES sailfish-office.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(FILES org.sailfish.documents.service DESTINATION ${CMAKE_INSTALL_PREFIX}/share/dbus-1/services)
install(FILES ${sailfishoffice_QML_SRCS} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sailfish-office
)
install(FILES ${engen_qm_file} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/translations)

install(FILES
    images/graphic-plugin-local.png
    images/graphic-plugin-network.png
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sailfish-office/images/
)
