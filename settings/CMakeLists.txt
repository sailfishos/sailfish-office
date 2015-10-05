install(FILES
    sailfish-office.json
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/jolla-settings/entries
)

install(FILES
    office.qml
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/jolla-settings/pages/sailfish-office
)
