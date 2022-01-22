# This source file is part of "glue project". Created by Andrey Vasiliev

find_path(ZZIP_INCLUDE_DIR NAMES zzip/zzip.h)

find_library(ZZIP_LIBRARY NAMES zziplib zzip zzip-0)

find_package_handle_standard_args(ZZIP DEFAULT_MSG ZZIP_LIBRARY ZZIP_INCLUDE_DIR)

if(ZZIP_FOUND)
    set(ZZIP_INCLUDE_DIRS ${ZZIP_INCLUDE_DIR})
    set(ZZIP_LIBRARIES ${ZZIP_LIBRARY})
else()
    set(ZZIP_INCLUDE_DIRS)
    set(ZZIP_LIBRARIES)
endif()

mark_as_advanced(ZZIP_LIBRARY ZZIP_INCLUDE_DIR)
