project(sailfish-office)

cmake_minimum_required(VERSION 2.8.6)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Qt4 REQUIRED QtDeclarative)
find_package(Booster REQUIRED)

include(${QT_USE_FILE})

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDES}
    ${BOOSTER_INCLUDE_DIR}
)

configure_file(config.h.in config.h)
    
set(sailfishoffice_SRCS
    main.cpp
    sailfishapplication/sailfishapplication.cpp
    models/documentlistmodel.cpp
    models/documentproviderlistmodel.cpp
    models/documentproviderplugin.cpp
    models/trackerdocumentprovider.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fPIC -pie -rdynamic")

qt4_automoc(${sailfishoffice_SRCS})
add_executable(sailfish-office ${sailfishoffice_SRCS})
target_link_libraries(sailfish-office stdc++ ${QT_LIBRARIES} ${BOOSTER_LIBRARY})

install(TARGETS sailfish-office DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES main.qml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sailfish-office)
install(FILES sailfish-office.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(DIRECTORY 
    cover
    pages
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sailfish-office
)
