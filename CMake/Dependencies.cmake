# This file is part of Glue Engine. Created by Andrey Vasiliev


if (NOT EXISTS ${CMAKE_BINARY_DIR}/cmake_lock AND NOT EXISTS ${CMAKE_BINARY_DIR}/build_finished)
    file(TOUCH ${CMAKE_BINARY_DIR}/cmake_lock)
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory build_tmp WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} .. WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build_tmp)
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --target ThirdParty WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build_tmp RESULT_VARIABLE BUILD_RESULT)
    if (BUILD_RESULT)
        file(TOUCH ${CMAKE_BINARY_DIR}/build_finished)
    endif ()
endif ()

execute_process(COMMAND ${CMAKE_COMMAND} -E rm -rf build_tmp WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/cmake_lock WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
