find_path(JOLLASIGNONUISERVICE_INCLUDE_DIR
    signonuiservice.h
    PATH_SUFFIXES
    libjollasignonuiservice
)

find_library(JOLLASIGNONUISERVICE_LIBRARY
    libjollasignonuiservice.so
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libjollasignonuiservice DEFAULT_MSG JOLLASIGNONUISERVICE_INCLUDE_DIR JOLLASIGNONUISERVICE_LIBRARY)

mark_as_advanced(JOLLASIGNONUISERVICE_INCLUDE_DIR JOLLASIGNONUISERVICE_LIBRARY)
