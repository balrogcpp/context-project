create_target(Target_SDL3 https://github.com/libsdl-org/SDL.git release-3.4.10 PATCH sdl2-3.4.4.patch)
externalproject_add(Target_SDL3
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_SDL3
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DSDL_SHARED_ENABLED_BY_DEFAULT=OFF
        -DSDL_X11_XTEST=OFF
        -DSDL_AUDIO=OFF
        -DSDL_TIMERS=OFF
        -DSDL_LOCALE=OFF
        -DSDL_FILESYSTEM=OFF
        -DSDL_TEST=OFF
        -DSDL_WERROR=OFF
        -DLIBC=ON
        -DSDL_LIBC=ON
        -DSDL_DIRECTX=OFF
        -DSDL_RENDER_D3D=OFF
        -DSDL_METAL=OFF
        -DSDL_RENDER_METAL=OFF
        -DSDL_VULKAN=ON
        -DHIDAPI=OFF
        -DSDL_HIDAPI=OFF
        -DSDL_HIDAPI_JOYSTICK=OFF
)


create_target(Target_pugixml https://github.com/zeux/pugixml.git v1.15)
externalproject_add(Target_pugixml
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_pugixml
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DPUGIXML_STATIC_CRT=ON
        -DBUILD_SHARED_LIBS=OFF
        -DPUGIXML_NO_XPATH=OFF
        -DPUGIXML_NO_STL=OFF
        -DPUGIXML_NO_EXCEPTIONS=OFF
)


create_target(Target_tinyxml2 https://github.com/leethomason/tinyxml2.git 11.0.0)
externalproject_add(Target_tinyxml2
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_tinyxml2
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -Dtinyxml2_BUILD_TESTING=OFF
        -Dtinyxml2_SHARED_LIBS=OFF
)


externalproject_add(Target_strawberryperl
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_strawberryperl
        URL https://github.com/StrawberryPerl/Perl-Dist-Strawberry/releases/download/SP_5382_32bit/strawberry-perl-5.38.2.1-32bit-portable.zip
        URL_HASH SHA256=c09a06d1e10e81a70b5f251938303b425680dad829ff738501ce96de99c640d9
        DOWNLOAD_EXTRACT_TIMESTAMP true
        PATCH_COMMAND ${CMAKE_COMMAND} -E true
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E true
        BUILD_COMMAND ${CMAKE_COMMAND} -E true
        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPS_SOURCE_LOCATION}/Target_strawberryperl/perl ${DEPS_ROOT}/perl
)


# Perl is required to build OpenSSL from sources. Perl expected to be bundled in UNIX OS
# Version should be at least 5.10.0, released in 2007. https://github.com/openssl/openssl/blob/master/NOTES-PERL.md
#create_target(Target_OpenSSL https://github.com/jimmy-park/openssl-cmake.git 3.6.2 PATCH openssl-3.6.2.patch)
if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    find_package(Perl QUIET)
    if (NOT PERL_FOUND)
        set(OPENSSL_DEPS Target_strawberryperl)
    endif ()
endif ()
externalproject_add(Target_OpenSSL
        EXCLUDE_FROM_ALL true
        DEPENDS ${OPENSSL_DEPS}
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_OpenSSL
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DCMAKE_PREFIX_PATH=${DEPS_ROOT}/perl
        -DOPENSSL_CONFIGURE_OPTIONS=--prefix=${DEPS_ROOT};
        -DOPENSSL_SOURCE=${DEPS_SOURCE_LOCATION}/Target_OpenSSL/openssl
        -DOPENSSL_INSTALL=ON
        -DOPENSSL_INSTALL_CERT=OFF
        -DBUILD_SHARED_LIBS=OFF
        -DOPENSSL_TARGET_VERSION=3.6.2
        -DOPENSSL_USE_CCACHE=OFF
        -DOPENSSL_TEST=OFF
        -DOPENSSL_NO_ASM=ON
)


create_target(Target_cpr https://github.com/libcpr/cpr.git 1.10.5 PATCH cpr-1.10.5.patch)
externalproject_add(Target_cpr
        EXCLUDE_FROM_ALL true
        DEPENDS Target_LibreSSL
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_cpr
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DBUILD_SHARED_LIBS=OFF
        -DCPR_ENABLE_CURL_HTTP_ONLY=ON
        -DCPR_FORCE_OPENSSL_BACKEND=ON
        -DCPR_USE_SYSTEM_CURL=OFF
        -DCPR_CURL_NOSIGNAL=OFF
        -DCURL_ZLIB=OFF
)


create_target(Target_http https://github.com/yhirose/cpp-httplib.git v0.46.1)
externalproject_add(Target_http
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_http
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DHTTPLIB_USE_OPENSSL_IF_AVAILABLE=OFF
        -DHTTPLIB_USE_ZLIB_IF_AVAILABLE=OFF
        -DHTTPLIB_USE_BROTLI_IF_AVAILABLE=OFF
        -DHTTPLIB_USE_CERTS_FROM_MACOSX_KEYCHAIN=OFF
)


create_target(Target_fmt https://github.com/fmtlib/fmt.git 12.1.0)
externalproject_add(Target_fmt
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_fmt
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DFMT_DOC=OFF
        -DFMT_TEST=OFF
        -DFMT_OS=ON
        -DFMT_MODULE=OFF
        -DFMT_SYSTEM_HEADERS=ON
        -DFMT_UNICODE=ON
)


create_target(Target_ghc https://github.com/gulrak/filesystem.git v1.5.14)
externalproject_add(Target_ghc
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_ghc
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DGHC_FILESYSTEM_BUILD_TESTING=OFF
        -DGHC_FILESYSTEM_BUILD_EXAMPLES=OFF
        -DGHC_FILESYSTEM_WITH_INSTALL=ON
        -DGHC_FILESYSTEM_BUILD_STD_TESTING=OFF
)


create_target(Target_SQLiteCpp https://github.com/SRombauts/SQLiteCpp.git 3.3.3)
externalproject_add(Target_SQLiteCpp
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_SQLiteCpp
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DSQLITECPP_BUILD_TESTS=OFF
        -DBUILD_SHARED_LIBS=OFF
        -DSQLITECPP_USE_STACK_PROTECTION=OFF
        -DSQLITECPP_INCLUDE_SCRIPT=OFF
        -DSQLITE_ENABLE_COLUMN_METADATA=OFF
        -DSQLITECPP_RUN_CPPCHECK=OFF
        -DSQLITECPP_RUN_CPPLINT=OFF
        -DSQLITECPP_RUN_DOXYGEN=OFF
        -DSQLITECPP_BUILD_EXAMPLES=OFF
        -DSQLITE_OMIT_LOAD_EXTENSION=OFF
        -DSQLITECPP_USE_ASAN=OFF
        -DSQLITECPP_INTERNAL_SQLITE=ON
        -DSQLITECPP_DISABLE_STD_FILESYSTEM=ON
)


create_target(Target_fkyaml https://github.com/fktn-k/fkYAML.git v0.4.2)
externalproject_add(Target_fkyaml
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_fkyaml
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DFK_YAML_INSTALL=ON
)


create_target(Target_ryaml https://github.com/biojppm/rapidyaml.git v0.10.0)
externalproject_add(Target_ryaml
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_ryaml
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DRYML_WITH_TAB_TOKENS=OFF
        -DRYML_DEFAULT_CALLBACKS=OFF
        -DRYML_DEFAULT_CALLBACK_USES_EXCEPTIONS=OFF
        -DRYML_USE_ASSERT=OFF
        -DRYML_BUILD_TOOLS=OFF
        -DRYML_BUILD_API=OFF
        -DRYML_DBG=OFF
        -DRYML_INSTALL=ON
)


create_target(Target_toml https://github.com/marzer/tomlplusplus.git v3.4.0)
externalproject_add(Target_toml
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_toml
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
)


create_target(Target_json https://github.com/nlohmann/json.git v3.12.0)
externalproject_add(Target_json
        EXCLUDE_FROM_ALL true
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_json
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DJSON_BuildTests=OFF
        -DJSON_MultipleHeaders=OFF
)


create_target(Target_enet https://github.com/lsalzman/enet.git v1.3.18)
externalproject_add(Target_enet
        EXCLUDE_FROM_ALL true
        BUILD_IN_SOURCE false
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_enet
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
)


create_target(Target_libarchive https://github.com/libarchive/libarchive.git v3.8.6)
externalproject_add(Target_libarchive
        EXCLUDE_FROM_ALL true
        BUILD_IN_SOURCE false
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_libarchive
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DENABLE_WERROR=OFF
        -DENABLE_TAR=ON
        -DENABLE_CPIO=OFF
        -DENABLE_CAT=OFF
        -DENABLE_UNZIP=OFF
        -DENABLE_TEST=OFF
        -DBUILD_SHARED_LIBS=OFF
        -DENABLE_INSTALL=ON
)

create_target(Target_googletest https://github.com/google/googletest.git v1.17.0)
externalproject_add(Target_googletest
        EXCLUDE_FROM_ALL true
        BUILD_IN_SOURCE false
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_googletest
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DBUILD_GMOCK=ON
        -DBUILD_SHARED_LIBS=OFF
        -DBENCHMARK_INSTALL_TOOLS=OFF
)


create_target(Target_benchmark https://github.com/google/benchmark.git v1.9.5)
externalproject_add(Target_benchmark
        EXCLUDE_FROM_ALL true
        BUILD_IN_SOURCE false
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_benchmark
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DBUILD_SHARED_LIBS=OFF
        -DBENCHMARK_ENABLE_TESTING=OFF
        -DBENCHMARK_ENABLE_EXCEPTIONS=OFF
        -DBENCHMARK_ENABLE_WERROR=OFF
        -DBENCHMARK_INSTALL_DOCS=OFF
)


externalproject_add(Target_OgreAudio
        EXCLUDE_FROM_ALL true
        DEPENDS Target_OGRE Target_Vorbis
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_OgreAudio
        GIT_REPOSITORY https://github.com/OGRECave/ogre-audiovideo.git
        GIT_TAG 2d373450b5c63849eeacfa1fefc9d101b52fd447
        GIT_SHALLOW false
        PATCH_COMMAND ${CMAKE_COMMAND} -P ${DEPS_PATCH_LOCATION}/PatchCMakeVersion.cmake
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DBUILD_VIDEOPLUGIN=OFF
        -DBUILD_AUDIOPLUGIN=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_Doxygen=ON
        -DOGGSOUND_THREADED=ON
        -DOGGSOUND_PYTHON=OFF
        -DUSE_EFX=0
)


externalproject_add(Target_OgreProcedural
        EXCLUDE_FROM_ALL true
        DEPENDS Target_OGRE
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_OgreProcedural
        GIT_REPOSITORY https://github.com/OGRECave/ogre-procedural.git
        GIT_TAG 6a0bfe7c0cae4e634e15c81bdbd62a0f954e2a92
        GIT_SHALLOW false
        PATCH_COMMAND ${CMAKE_COMMAND} -P ${DEPS_PATCH_LOCATION}/PatchCMakeVersion.cmake
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DOgreProcedural_BUILD_SAMPLES=OFF
        -DOgreProcedural_BUILD_TESTS=OFF
        -DOgreProcedural_BUILD_DOCS=OFF
        -DOgreProcedural_STATIC=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_Freetype=ON
)


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


# Ninja and Python3 required for Qt build https://doc.qt.io/qt-6/linux-building.html
# Unix expected to have python by default
# Qt QML build creates very long tmp files, symbolic link is a way to avoid compilation error
# Limitation comes with MSVC toolchain, it can't handle path over 260 characters
# Another limitation comes from Windows OS. While it is possible to fix this via regedit, we can't relay on it
create_target(Target_Qt6 https://github.com/qt/qt5.git v6.11.1 MODULES "qtbase;qtdeclarative;qtshadertools")
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
    endif ()
    execute_process(COMMAND cmd.exe /c mklink /J "${dstDir}" "${srcDir}")
else ()
    set (QT_SYMLINK ${DEPS_PREFIX_LOCATION})
endif ()

externalproject_add(Target_Qt6
        EXCLUDE_FROM_ALL true
        DEPENDS Target_cpython
        PREFIX ${QT_SYMLINK}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_Qt6
        DOWNLOAD_COMMAND ${DOWNLOAD_PLACEHOLDER}
        PATCH_COMMAND ${PATCH_PLACEHOLDER}
        CMAKE_GENERATOR Ninja
        CMAKE_ARGS
        ${EXTERNAL_PROJECT_CFG}
        -DCMAKE_MAKE_PROGRAM=${NINJA_EXECUTABLE}
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
