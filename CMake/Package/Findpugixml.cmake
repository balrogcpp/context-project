# - Find PugiXML library
# Find the native PugiXML includes
# This module defines
#  PugiXML_INCLUDE_DIR, where to find PugiXML.hpp
#  PugiXML_FOUND, If false, do not try to use PugiXML.

find_path(PUGIXML_INCLUDE_DIR pugixml.hpp)

find_library(PUGIXML_LIBRARY NAMES pugixml)

# handle the QUIETLY and REQUIRED arguments and set PugiXML_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(pugixml REQUIRED_VARS PUGIXML_INCLUDE_DIR PUGIXML_LIBRARY)

mark_as_advanced(PUGIXML_INCLUDE_DIR)
mark_as_advanced(PUGIXML_LIBRARY)
