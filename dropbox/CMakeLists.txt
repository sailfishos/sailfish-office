set(dropbox_lib_SRCS
    libs/droprestapi.cc
    libs/filetransferitem.cc
    libs/folderitem.cc
    libs/json.cc
    libs/listmodel.cc
    libs/networkcontroller.cc
    libs/oauth.cc
    libs/options.cc
)

#automoc4_add_library(sailfishofficedropboxlib SHARED ${dropbox_lib_SRCS})
#target_link_libraries(sailfishofficedropboxlib stdc++ ${QT_LIBRARIES})

#install(TARGETS sailfishofficedropboxlib DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

set(dropbox_plugin_SRCS
    controller.cc
    dropboxplugin.cpp)

automoc4_add_library(sailfishofficedropboxplugin MODULE ${dropbox_plugin_SRCS} ${dropbox_lib_SRCS})
target_link_libraries(sailfishofficedropboxplugin stdc++ ${QT_LIBRARIES})# sailfishofficedropboxlib)

install(TARGETS sailfishofficedropboxplugin DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/qt4/imports/Sailfish/Office/Dropbox)

install(FILES
    qmldir
    FileNavigator.qml
    InfoBanner.qml
    LoginPage.qml
    SailWebView.qml
    SetupPage.qml
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/qt4/imports/Sailfish/Office/Dropbox
)
