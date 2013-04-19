find_path(POPPLER_INCLUDE_DIR
    poppler-config.h
    PATH_SUFFIXES
    poppler
)

find_path(POPPLER_QT4_INCLUDE_DIR
    poppler-qt4.h
    PATH_SUFFIXES
    poppler/qt4
)

find_library(POPPLER_LIBRARY
    NAMES libpoppler.so
)

find_library(POPPLER_QT4_LIBRARY
    NAMES libpoppler-qt4.so
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Poppler DEFAULT_MSG POPPLER_INCLUDE_DIR POPPLER_QT4_INCLUDE_DIR POPPLER_LIBRARY POPPLER_QT4_LIBRARY)

mark_as_advanced(POPPLER_INCLUDE_DIR POPPLER_QT4_INCLUDE_DIR POPPLER_LIBRARY POPPLER_QT4_LIBRARY)
 
