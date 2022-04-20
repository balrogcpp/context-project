# This file is part of Glue Engine. Created by Andrey Vasiliev

include(ThirdParty)
include(ExternalProject)

add_custom_target(BuildAssets
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/Tmp
        COMMAND ${CMAKE_COMMAND} -P CMake/FlatZipAssets.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating assets and programs zip package..."
        USES_TERMINAL
        )

list(APPEND GLUE_DEPENDENCY_TARGETS Target_asio)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_Lua)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_sol2)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_json)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_Vorbis)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_OGRE)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_SDL2)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_Bullet)

add_custom_target(ThirdParty DEPENDS ${GLUE_DEPENDENCY_TARGETS})
