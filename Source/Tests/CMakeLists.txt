# This source file is part of "glue project". Created by Andrey Vasiliev

set(CMAKE_FOLDER Tests)
find_package(GTest QUIET)
if (GTest_FOUND AND NOT CMAKE_CROSSCOMPILING)
    message("GoogleTest found. Test targets are available")
    enable_testing()
    include(GoogleTest)

    add_executable(TestGlue EXCLUDE_FROM_ALL src/TestHello.cpp)
    target_link_libraries(TestGlue ${GTEST_BOTH_LIBRARIES})
    include_directories(TestGlue ${GTEST_INCLUDE_DIRS})

    gtest_discover_tests(TestGlue)
    target_link_directories(TestGlue PUBLIC ${GLUE_EXTERNAL_LIB_DIR} ${GLUE_EXTERNAL_LIB_DIR}/OGRE ${GLUE_LIB_DIR})
else ()
    message("GoogleTest not found. Test targets are not available")
endif ()
