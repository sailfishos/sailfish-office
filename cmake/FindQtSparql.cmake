find_path(QTSPARQL_INCLUDE_DIR
    QtSparql
    PATH_SUFFIXES
    QtSparql
    qt4/QtSparql
)

find_library(QTSPARQL_LIBRARY
    QtSparql
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QtSparql DEFAULT_MSG QTSPARQL_INCLUDE_DIR QTSPARQL_LIBRARY)

mark_as_advanced(QTSPARQL_INCLUDE_DIR QTSPARQL_LIBRARY)
