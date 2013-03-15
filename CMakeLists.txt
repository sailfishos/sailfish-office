project(Documents)

cmake_minimum_required(VERSION 2.8.6)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Qt4 REQUIRED QtDeclarative)
find_package(Silica REQUIRED)
find_package(Booster REQUIRED)

include(${QT_USE_FILE})

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDES}
    ${BOOSTER_INCLUDE_DIR}
)

configure_file(config.h.in config.h)
    
set(documents_SRCS
    main.cpp
    sailfishapplication/sailfishapplication.cpp
    models/documentlistmodel.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fPIC -pie -rdynamic")

qt4_automoc(${documents_SRCS})
add_executable(documents ${documents_SRCS})
target_link_libraries(documents stdc++ ${QT_LIBRARIES} ${BOOSTER_LIBRARY})

install(TARGETS documents DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES main.qml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/documents)
install(FILES Documents.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(DIRECTORY 
    cover
    pages
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/documents
)
