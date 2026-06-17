# include guard
if (_build_openssl_included)
    return()
endif (_build_openssl_included)
set(_build_openssl_included true)


if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
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
set(OPENSSL_DEP Target_strawberryperl)
endif ()


externalproject_add(Target_OpenSSL3
        EXCLUDE_FROM_ALL true
        DEPENDS ${OPENSSL_DEP}
        PREFIX ${DEPS_PREFIX_LOCATION}
        DOWNLOAD_DIR ${DEPS_SOURCE_LOCATION}
        SOURCE_DIR ${DEPS_SOURCE_LOCATION}/Target_OpenSSL
        DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E chdir ${DEPS_SOURCE_LOCATION}
        ${CMAKE_COMMAND} -DTARGET=Target_OpenSSL
        -DREPO=https://github.com/jimmy-park/openssl-cmake.git
        -DTAG=3.6.2
        -DPATCH=${DEPS_PATCH_LOCATION}/openssl-3.6.2.patch
        -P ${CMAKE_CURRENT_SOURCE_DIR}/CMake/FetchSource.cmake
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
