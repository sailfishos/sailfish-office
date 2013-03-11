find_path(BOOSTER_INCLUDE_DIR
    booster.h
    PATH_SUFFIXES
    applauncherd
)

find_library(BOOSTER_LIBRARY
    NAMES applauncherd
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Booster DEFAULT_MSG BOOSTER_INCLUDE_DIR BOOSTER_LIBRARY)

mark_as_advanced(BOOSTER_INCLUDE_DIR BOOSTER_LIBRARY)
 
