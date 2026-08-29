cmake_minimum_required(VERSION 3.18)

project(mpy C)

include(CTest)

set(EMBED_DIR "micropython_embed")

set(MPY_EXE "${PROJECT_NAME}")
set(MPY_LIB ${PROJECT_NAME})

set(MPY_COMPILE_OPTIONS
	-Wall 
	-Og
)

file(GLOB_RECURSE MPY_SOURCE_FILES ${EMBED_DIR}/*.c)
file(GLOB_RECURSE MPY_HEADER_FILES ${EMBED_DIR}/*.h)


# defaults to building static-libs
# can build shared if specified here
# or -DBUILD_SHARED_LIBS option is set
add_library(${MPY_LIB} # SHARED
	${MPY_SOURCE_FILES}
)

target_include_directories(${MPY_LIB}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${EMBED_DIR}
	${EMBED_DIR}/extmod
	${EMBED_DIR}/genhdr
	${EMBED_DIR}/port
	${EMBED_DIR}/py
	${EMBED_DIR}/shared
)

target_compile_options(${MPY_LIB}
	PUBLIC
	${MPY_COMPILE_OPTIONS}
)

#add_executable(${MPY_EXE}
#    main.c
#    # below is required for static library case due to extern definition
#    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${EMBED_DIR}/py/mpstate.c>
#)

#target_link_libraries(${MPY_EXE}
#	${MPY_LIB}
#)

#target_include_directories(${MPY_EXE}
#	PUBLIC
#	${CMAKE_CURRENT_SOURCE_DIR}
#	${EMBED_DIR}
#	${EMBED_DIR}/extmod
#	${EMBED_DIR}/genhdr
#	${EMBED_DIR}/port
#	${EMBED_DIR}/py
#	${EMBED_DIR}/shared
#)

#add_test(NAME ${MPY_EXE} COMMAND ${MPY_EXE})


#target_compile_options(${MPY_EXE}
#	PUBLIC
#	${MPY_COMPILE_OPTIONS}
#)
