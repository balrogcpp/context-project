project(theora C)
cmake_minimum_required(VERSION 2.9)

# https://github.com/xiph/theora/issues/10#issuecomment-591894142


find_package(Ogg REQUIRED)


set(LIBTHEORA_SOURCE ${LIBTHEORA_DEC} "${CMAKE_SOURCE_DIR}/lib/apiwrapper.c"
        "${CMAKE_SOURCE_DIR}/lib/bitpack.c" "${CMAKE_SOURCE_DIR}/lib/dequant.c"
        "${CMAKE_SOURCE_DIR}/lib/fragment.c" "${CMAKE_SOURCE_DIR}/lib/idct.c"
        "${CMAKE_SOURCE_DIR}/lib/info.c" "${CMAKE_SOURCE_DIR}/lib/internal.c"
        "${CMAKE_SOURCE_DIR}/lib/state.c" "${CMAKE_SOURCE_DIR}/lib/quant.c"
        "${CMAKE_SOURCE_DIR}/lib/decapiwrapper.c" "${CMAKE_SOURCE_DIR}/lib/decinfo.c"
        "${CMAKE_SOURCE_DIR}/lib/decode.c" "${CMAKE_SOURCE_DIR}/lib/huffdec.c")

add_library(theoradec STATIC ${LIBTHEORA_SOURCE})
target_include_directories(theoradec PRIVATE ${OGG_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include")
#target_compile_options(theoradec PRIVATE -Wno-shift-negative-value -Wno-shift-op-parentheses)


install(TARGETS theoradec)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/theora TYPE INCLUDE FILES_MATCHING PATTERN "*.h")
