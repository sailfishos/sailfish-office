find_path(QTSPARQL_INCLUDE_DIR
    QtSparql
    PATH_SUFFIXES
    Qt5Sparql
)

find_library(QTSPARQL_LIBRARY
    Qt5Sparql
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QtSparql DEFAULT_MSG QTSPARQL_INCLUDE_DIR QTSPARQL_LIBRARY)

mark_as_advanced(QTSPARQL_INCLUDE_DIR QTSPARQL_LIBRARY)
