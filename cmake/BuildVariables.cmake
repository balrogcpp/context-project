# include guard
if (_buildvariables_included)
    return()
endif (_buildvariables_included)
set(_buildvariables_included true)

list(APPEND CMAKE_PREFIX_PATH ${DEPS_ROOT})
list(APPEND CMAKE_FIND_ROOT_PATH ${DEPS_ROOT})

macro(find_package_static package)
    set(_OLD_FIND_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".a" ".so" ".sl" ".dylib" ".dll.a")
    find_package(${package})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_OLD_FIND_SUFFIXES})
    unset(_OLD_FIND_SUFFIXES)
endmacro()


# check dependencies
#find_package_static(Lua)
#find_package_static(sol2)
find_package_static(OpenAL)
find_package_static(Ogg)
find_package_static(Vorbis)
find_package_static(SDL2)
if (BUILD_CURL_LIBS)
    find_package_static(CURL)
    find_package_static(cpr)
endif ()
if (BUILD_NETWORK_LIBS)
    find_package_static(Protobuf)
    find_package_static(GameNetworkingSockets)
endif ()
set(OGRE_STATIC 1)
set(OGRE_IGNORE_ENV 1)
find_package(OGRE)


# engine
set(ENGINE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/source/Engine)
set(ENGINE_LINK_DIRS ${DEPS_ROOT}/lib ${DEPS_ROOT}/lib/OGRE)
set(ENGINE_INCLUDE_DIRS
        ${ENGINE_SOURCE_DIR}
        ${ENGINE_SOURCE_DIR}/Code
        ${CMAKE_BINARY_DIR}/source
        ${DEPS_ROOT}/include
)


if (OGRE_FOUND)
    list(APPEND ENGINE_LIBRARIES ${OGRE_LIBRARIES})
    list(APPEND ENGINE_INCLUDE_DIRS ${OGRE_INCLUDE_DIRS})
endif ()
if (Lua_FOUND)
    list(APPEND ENGINE_LIBRARIES ${LUA_LIBRARY})
    list(APPEND ENGINE_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
endif ()
if (Vorbis_FOUND)
    list(APPEND ENGINE_LIBRARIES ${VORBIS_LIBRARIES})
    list(APPEND ENGINE_INCLUDE_DIRS ${VORBIS_INCLUDE_DIRS})
endif ()
if (Ogg_FOUND)
    list(APPEND ENGINE_LIBRARIES ${OGG_LIBRARIES})
    list(APPEND ENGINE_INCLUDE_DIRS ${OGG_INCLUDE_DIRS})
endif ()
if (OpenAL_FOUND)
    list(APPEND ENGINE_LIBRARIES ${OPENAL_LIBRARY})
    list(APPEND ENGINE_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
endif ()
if (TARGET SDL2::SDL2-static)
    list(APPEND ENGINE_LIBRARIES SDL2::SDL2-static)
endif ()
if (cpr_FOUND)
    list(APPEND ENGINE_LIBRARIES ${cpr_LIBRARY})
    list(APPEND ENGINE_INCLUDE_DIRS ${cpr_INCLUDE_DIR})
endif ()
if (CURL_FOUND)
    list(APPEND ENGINE_LIBRARIES CURL::libcurl)
endif ()
if (GameNetworkingSockets_FOUND)
    list(APPEND ENGINE_LIBRARIES GameNetworkingSockets::static)
endif ()
