# include guard
if (_build_qt_included)
    return()
endif (_build_qt_included)
set(_build_qt_included true)


if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
externalproject_add(Target_cpython
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_cpython
        URL https://github.com/astral-sh/python-build-standalone/releases/download/20260602/cpython-3.10.20+20260602-i686-pc-windows-msvc-install_only_stripped.tar.gz
        URL_HASH SHA256=63befed008b7d4a3cc80f0f20e76fac0899d87bb5832ff5f033c18cc353e077c
        DOWNLOAD_EXTRACT_TIMESTAMP true
        PATCH_COMMAND ${CMAKE_COMMAND} -E true
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E true
        BUILD_COMMAND ${CMAKE_COMMAND} -E true
        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPS_SOURCE_LOCATION}/Target_cpython ${DEPS_ROOT}/python
)
set(QT_DEP Target_cpython)
endif ()


# Ninja and Python3 required for Qt build https://doc.qt.io/qt-6/linux-building.html
# Unix expected to have python by default
# Qt QML build creates very long tmp files, symbolic link is a way to avoid compilation error
# Limitation comes with MSVC toolchain, it can't handle path over 260 characters
# Another limitation comes from Windows OS. While it is possible to fix this via regedit, we can't relay on it
if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set (QT_SYMLINK "$ENV{SystemRoot}/Temp/Qt6")
    set (QT_PREFIX ${DEPS_PREFIX_LOCATION})
    if (NOT EXISTS ${QT_PREFIX})
        file(MAKE_DIRECTORY ${QT_PREFIX})
    endif ()
    file(TO_NATIVE_PATH ${QT_PREFIX} srcDir)
    file(TO_NATIVE_PATH ${QT_SYMLINK} dstDir)
    if (IS_SYMLINK ${QT_SYMLINK})
        execute_process(COMMAND cmd.exe /c rmdir "${dstDir}")
    elseif (EXISTS ${QT_SYMLINK})
        file(REMOVE_RECURSE ${dstDir})
    endif ()
    execute_process(COMMAND cmd.exe /c mklink /J "${dstDir}" "${srcDir}")
else ()
    set (QT_SYMLINK ${DEPS_PREFIX_LOCATION})
endif ()
find_program(NINJA_EXECUTABLE ninja REQUIRED)
set (QT_MODULES "qtbase,qtdeclarative,qtshadertools")
externalproject_add(Target_Qt6
        LIST_SEPARATOR ,
        EXCLUDE_FROM_ALL true
        DEPENDS ${QT_DEP}
        PREFIX ${QT_SYMLINK}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_Qt6
        DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E chdir ${DEPS_SOURCE_LOCATION}
        ${CMAKE_COMMAND} -DTARGET=Target_Qt6
        -DREPO=https://github.com/qt/qt5.git
        -DTAG=v6.11.1
        -DMODULES:STRING=${QT_MODULES}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/CMake/FetchSource.cmake
        CMAKE_GENERATOR Ninja
        CMAKE_CACHE_ARGS -DQT_BUILD_SUBMODULES:STRING=${QT_MODULES}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DCMAKE_MAKE_PROGRAM=${NINJA_EXECUTABLE}
        -DQT_ALLOW_SYMLINK_IN_PATHS=ON
        -DPython_ROOT_DIR=${DEPS_ROOT}/python
        -DCMAKE_DISABLE_FIND_PACKAGE_OpenSSL=ON
        -DQT_GENERATE_SBOM=OFF
        -DQT_BUILD_EXAMPLES=OFF
        -DQT_BUILD_TESTS=OFF
        -DQT_FEATURE_debug_and_release=OFF
        -DQT_FEATURE_testlib=OFF
        -DQT_FEATURE_sql=OFF
        -DQT_FEATURE_printsupport=OFF
        -DQT_FEATURE_xml=OFF
        -DQT_FEATURE_dbus=OFF
        -DQT_FEATURE_concurrent=OFF
        -DQT_FEATURE_qml_debug=OFF
)
