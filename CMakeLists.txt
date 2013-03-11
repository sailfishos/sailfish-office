project(Documents)

cmake_minimum_required(VERSION 2.8.6)

set(CMAKE_INSTALL_PREFIX /opt/sdk)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Qt4 REQUIRED QtDeclarative)
find_package(Silica REQUIRED)
find_package(Booster REQUIRED)

add_definitions(-DHAVE_BOOSTER)

include(${QT_USE_FILE})

include_directories(
    sailfishapplication
    ${QT_INCLUDES}
    ${BOOSTER_INCLUDE_DIR}
)
    
set(documents_SRCS
    main.cpp
    sailfishapplication/sailfishapplication.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pie -rdynamic")

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
