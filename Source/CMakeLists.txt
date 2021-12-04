include(CppFlags)

set(GLUE_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/glue)
set(GLUE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Source/Glue/include)

list(APPEND GLUE_INCLUDE_DIRS
        ${GLUE_INCLUDE_DIR}
        ${GLUE_EXTERNAL_INSTALL_LOCATION}/include
        ${BULLET_INCLUDE_DIR}
        ${OGRE_INCLUDE_DIRS}
        ${OGRE_INCLUDE_DIRS}/Overlay
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GL3Plus
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GL3Plus
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2
        ${OGRE_INCLUDE_DIRS}/RenderSystems/GLES2/GLSLES
        ${OGRE_INCLUDE_DIRS}/Plugins/Assimp
        )

set(SAMPLE_EXE sample)
file(GLOB_RECURSE SAMPLE_SOURCE_FILES Glue/src/*.cpp Sample/src/*.cpp)

if (NOT ANDROID)
    add_executable(Sample WIN32 ${SAMPLE_SOURCE_FILES})
else ()
    add_library(Sample SHARED ${SAMPLE_SOURCE_FILES})
    set_target_properties(Sample PROPERTIES OUTPUT_NAME "main")
endif ()

include_directories(Sample Source/Sample/include ${GLUE_INCLUDE_DIRS})
target_link_directories(Sample PUBLIC ${GLUE_EXTERNAL_LIB_DIR} ${GLUE_EXTERNAL_LIB_DIR}/OGRE ${GLUE_LIB_DIR})
target_precompile_headers(Sample PUBLIC Glue/include/pch.h)
set_target_properties(Sample PROPERTIES OUTPUT_NAME "Sample")

if (MINGW)
    list(APPEND SYSTEM_LIBRARIES zlibstatic imagehlp dinput8 dxguid dxerr8 user32 gdi32 imm32 winmm ole32 oleaut32 shell32 version uuid setupapi hid)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    #jemalloc fix problems with memory bloat on linux
    list(APPEND SYSTEM_LIBRARIES X11 Xrandr dl jemalloc.a)
elseif (ANDROID)
    list(APPEND SYSTEM_LIBRARIES SDL2::hidapi android atomic EGL GLESv1_CM GLESv2 log OpenSLES)
elseif (MSVC)
    list(APPEND SYSTEM_LIBRARIES winmm Version imm32 Setupapi)
endif ()
if (NOT (MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(OGRE_CODEC_ASSIMP Codec_AssimpStatic)
else ()
    set(OGRE_CODEC_ASSIMP Codec_AssimpStatic_d)
endif ()

target_link_libraries(Sample PUBLIC
        ${OGRE_LIBRARIES}
        ${OGRE_CODEC_ASSIMP}
        ${ASSIMP_LIBRARIES}
        ${ASSIMP_IRRXML_LIBRARY}
        ${BULLET_LIBRARIES}
        ${PNG_LIBRARY}
        vorbisfile
        vorbis
        vorbisenc
        Ogg::ogg
        pugixml
        SDL2::SDL2-static
        ${LUA_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${SYSTEM_LIBRARIES}
        )

install(TARGETS Sample RUNTIME DESTINATION .)

#option(GLUE_BUILD_STATIC_LIBRARY "Build static library" OFF)
#if (GLUE_BUILD_STATIC_LIBRARY)
#    file(GLOB_RECURSE GLUE_ENGINE_SOURCE_FILES Source/*.cpp Source/*.h Source/*.hpp)
#    add_library(glue STATIC ${GLUE_ENGINE_SOURCE_FILES})
#    include_directories(glue ${GLUE_INCLUDE_DIRS})
#    set_target_properties(glue PROPERTIES OUTPUT_NAME "glue-static")
#    target_precompile_headers(glue PUBLIC Source/Glue/include/pch.h)
#endif ()


find_package(GTest QUIET)
if (GTest_FOUND AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    include(GoogleTest)

    add_executable(TestGlue Tests/src/TestHello.cpp)
    target_link_libraries(TestGlue ${GTEST_BOTH_LIBRARIES})

    gtest_discover_tests(TestGlue)
    target_link_directories(TestGlue PUBLIC ${GLUE_EXTERNAL_LIB_DIR} ${GLUE_EXTERNAL_LIB_DIR}/OGRE ${GLUE_LIB_DIR})
else ()
    message("Google test not found. Test targets are not available.")
endif ()
