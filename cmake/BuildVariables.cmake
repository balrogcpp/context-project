# include guard
if (_buildvariables_included)
    return()
endif (_buildvariables_included)
set(_buildvariables_included true)


# insert_dependency macro
include(InsertDependency)
include(Platform)


# https://stackoverflow.com/questions/54587052/cmake-on-mac-could-not-find-threads-missing-threads-found
if (APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
endif ()


list(APPEND CMAKE_PREFIX_PATH ${DEPS_ROOT})
list(APPEND CMAKE_FIND_ROOT_PATH ${DEPS_ROOT})


# check dependencies
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(X11 QUIET)
endif ()
find_package(Threads)
insert_dependency_static(Bullet)
insert_dependency_static2(Lua LUA_LIBRARY)
insert_dependency_static(sol2)
insert_dependency2(OpenAL OPENAL_FOUND)
insert_dependency_static(Ogg)
insert_dependency_static(Vorbis)
insert_dependency_static(SDL2)
insert_dependency_static(CURL)
insert_dependency_static(cpr)
insert_dependency_static(Protobuf)
insert_dependency_static(GameNetworkingSockets)
set(OGRE_STATIC 1)
set(OGRE_IGNORE_ENV 1)
insert_dependency_static(OGRE)


# engine
set(ENGINE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/source/Engine)

set(ENGINE_INCLUDE_DIRS
        ${ENGINE_SOURCE_DIR}
        ${ENGINE_SOURCE_DIR}/Code
        ${CMAKE_BINARY_DIR}
        ${DEPS_ROOT}/include
)

set(ENGINE_LINK_DIRS ${DEPS_ROOT}/lib ${DEPS_ROOT}/lib/OGRE)

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
if (Bullet_FOUND)
    list(APPEND ENGINE_LIBRARIES ${BULLET_LIBRARIES})
    list(APPEND ENGINE_INCLUDE_DIRS ${BULLET_INCLUDE_DIRS})
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
