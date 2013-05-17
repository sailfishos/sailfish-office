project(sailfish-office)

cmake_minimum_required(VERSION 2.8.6)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Qt4 REQUIRED QtDeclarative QtDBus)
find_package(Automoc4 REQUIRED)
find_package(Booster REQUIRED)
find_package(QtSparql REQUIRED)
find_package(JollaSignonUiService REQUIRED)

include(${QT_USE_FILE})

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDES}
    ${BOOSTER_INCLUDE_DIR}
    ${QTSPARQL_INCLUDE_DIR}
    ${JOLLASIGNONUISERVICE_INCLUDE_DIR}
)

set(CALLIGRA_QML_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib/calligra/imports CACHE PATH "The location of the Calligra QtQuick Components")
configure_file(config.h.in config.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fPIC -pie -rdynamic -Wall")

add_subdirectory(plugin)
add_subdirectory(pdf)

set(sailfishoffice_SRCS
    main.cpp
    sailfishapplication.cpp
    models/documentlistmodel.cpp
    models/documentproviderlistmodel.cpp
    models/documentproviderplugin.cpp
    models/trackerdocumentprovider.cpp
)

automoc4_add_executable(sailfish-office ${sailfishoffice_SRCS})
target_link_libraries(sailfish-office stdc++ ${QT_LIBRARIES} ${BOOSTER_LIBRARY} ${QTSPARQL_LIBRARY} ${JOLLASIGNONUISERVICE_LIBRARY})

install(TARGETS sailfish-office DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES sailfish-office.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(FILES org.sailfish.documents.service DESTINATION ${CMAKE_INSTALL_PREFIX}/share/dbus-1/services)
install(FILES
    qml/CoverPage.qml
    qml/FileListPage.qml
    qml/Main.qml
    qml/StartPage.qml
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sailfish-office
)
install(FILES
    images/graphic-plugin-local.png
    images/graphic-plugin-network.png
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sailfish-office/images/
)
