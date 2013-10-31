find_path(POPPLER_INCLUDE_DIR
    poppler-version.h
    PATH_SUFFIXES
    poppler/cpp
)

find_path(POPPLER_QT5_INCLUDE_DIR
    poppler-qt5.h
    PATH_SUFFIXES
    poppler/qt5
)

find_library(POPPLER_LIBRARY
    NAMES libpoppler.so
)

find_library(POPPLER_QT5_LIBRARY
    NAMES libpoppler-qt5.so
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Poppler DEFAULT_MSG POPPLER_INCLUDE_DIR POPPLER_QT5_INCLUDE_DIR POPPLER_LIBRARY POPPLER_QT5_LIBRARY)

mark_as_advanced(POPPLER_INCLUDE_DIR POPPLER_QT5_INCLUDE_DIR POPPLER_LIBRARY POPPLER_QT5_LIBRARY)
 
