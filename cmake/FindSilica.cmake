find_path(SILICA_IMPORTS_DIR
    ApplicationWindow.qml
    PATHS
    /usr/lib/qt4/imports/Sailfish/Silica
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Silica DEFAULT_MSG SILICA_IMPORTS_DIR)

mark_as_advanced(SILICA_IMPORTS_DIR)
 
