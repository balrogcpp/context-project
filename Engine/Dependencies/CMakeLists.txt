# This file is part of Glue Engine. Created by Andrey Vasiliev

include(ThirdParty)


# list of dependency targets
list(APPEND GLUE_DEPENDENCY_TARGETS Target_asio)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_Lua)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_sol2)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_json)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_pugixml)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_FreeType)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_Vorbis)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_OGRE)
list(APPEND GLUE_DEPENDENCY_TARGETS Target_Bullet)
if (NOT EMSCRIPTEN)
    add_dependencies(Target_Ogg Target_OpenAL)
    list(APPEND GLUE_DEPENDENCY_TARGETS Target_SDL2)
endif ()


# proxy target
add_custom_target(Dependencies DEPENDS ${GLUE_DEPENDENCY_TARGETS})
