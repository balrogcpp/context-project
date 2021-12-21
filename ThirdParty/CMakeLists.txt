include(ThirdParty)
include(ExternalProject)

add_custom_target(Assets
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR} ${GIT_EXECUTABLE} clone --depth 1 https://github.com/balrogcpp/scenes.git Assets
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/Assets/.git
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/Tmp
        COMMAND ${CMAKE_COMMAND} -P CMake/FlatZipAssets.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Fetching assets..."
        USES_TERMINAL
        )

add_custom_target(Assets_SSH
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR} ${GIT_EXECUTABLE} clone --depth 1 git@github.com:balrogcpp/scenes.git Assets
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/Tmp
        COMMAND ${CMAKE_COMMAND} -P CMake/FlatZipAssets.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Fetching assets..."
        USES_TERMINAL
        )

add_custom_target(AssetsBuild
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/Tmp
        COMMAND ${CMAKE_COMMAND} -P CMake/FlatZipAssets.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating assets and programs zip package..."
        USES_TERMINAL
        )

list(APPEND GLUE_DEPENDENCY_TARGETS Target_Lua)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_sol)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_Vorbis)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_OGRE)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_Bullet)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_GoogleTest)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND GLUE_DEPENDENCY_TARGETS Target_jemalloc)
endif ()

add_custom_target(ThirdParty DEPENDS ${GLUE_DEPENDENCY_TARGETS})
