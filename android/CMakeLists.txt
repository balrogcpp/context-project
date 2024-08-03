# Android
set(ANDROID_HOME $ENV{ANDROID_HOME})
set(ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})
if ((IS_DIRECTORY ${ANDROID_HOME}) OR (IS_DIRECTORY ${ANDROID_SDK_ROOT}))
    message(STATUS "Android SDK found. Adding Android build targets")

    if (WIN32)
        set(GRADLEW_COMMAND gradlew.bat)
    else ()
        set(GRADLEW_COMMAND sh gradlew)
    endif ()
    if (${CMAKE_BUILD_TYPE_NORMALISED} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE_NORMALISED} STREQUAL "MinSizeRel")
        set(GRADLE_BUILD_TYPE "Release")
    elseif (${CMAKE_BUILD_TYPE_NORMALISED} STREQUAL "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE_NORMALISED} STREQUAL "Debug")
        set(GRADLE_BUILD_TYPE "Debug")
    endif ()

    add_custom_target(GradleBuild
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E env GIT_SHA1=${GIT_SHA_SHORT} ${GRADLEW_COMMAND} assemble${GRADLE_BUILD_TYPE}
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E env GIT_SHA1=${GIT_SHA_SHORT} ${GRADLEW_COMMAND} clean
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E env GIT_SHA1=${GIT_SHA_SHORT} ${GRADLEW_COMMAND} assemble${GRADLE_BUILD_TYPE}
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E copy_directory Android/build/outputs/apk ${ARTIFACT_DIR}
    )

    add_custom_target(GradleClear COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_COMMAND} -E rm -rf .gradle Android/.cxx Android/build Android/assets)
else ()
    message(STATUS "Android SDK not found. Skipping Android build targets")
endif ()
