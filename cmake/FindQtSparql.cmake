if(SAILFISH_SCRATCHBOX)
    set(QTSPARQL_INCLUDES_DIR /usr/include/QtSparql)
    message("!! Assuming QtSparql in /usr/include/QtSparql - Ensure libqtsparql-devel is installed in the Sailfish SDK scratchbox target, as well as libqtsparql and libqtsparql-tracker on the runtime VM")
else()
    find_path(QTSPARQL_INCLUDES_DIR
        QtSparql
        PATHS
        /usr/include
        /usr/include/QtSparql
        /usr/include/qt4/QtSparql
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(QtSparql DEFAULT_MSG QTSPARQL_INCLUDES_DIR)
endif(SAILFISH_SCRATCHBOX)

mark_as_advanced(QTSPARQL_INCLUDES_DIR)
