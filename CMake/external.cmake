include(ExternalProject)

externalproject_add(target-gtest
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/google/googletest.git
                    GIT_TAG release-1.8.1
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-gbenchmark
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/google/benchmark.git
                    GIT_TAG v1.5.0
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-glm
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/g-truc/glm.git
                    GIT_TAG 0.9.9.6
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DBUILD_SHARED_LIBS=false
                    -DBUILD_STATIC_LIBS=false
                    -DGLM_TEST_ENABLE=false
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-glm ${CMAKE_COMMAND}
                    -E copy_directory glm ${CONTEXT_EXTERNAL_INSTALL_LOCATION}/include/glm
                    )

externalproject_add(target-angelscript
                    EXCLUDE_FROM_ALL true
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/codecat/angelscript-mirror.git
                    GIT_TAG 3a7440083b039bd87e79956365f5e9e938af32a3
                    GIT_SHALLOW false
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-angelscript-build
                    ${CMAKE_COMMAND}
                    -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-angelscript/sdk/angelscript/projects/cmake
                    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-angelscript-build
                    ${CMAKE_COMMAND} --build ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-angelscript-build
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-angelscript-build
                    ${CMAKE_COMMAND} --build ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-angelscript-build --target install
                    )

externalproject_add(target-theora
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-ogg
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/xiph/theora.git
                    GIT_TAG v1.2.0alpha1
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-theora ./autogen.sh
                    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-theora ${CONTEXT_MAKE}
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E chdir ${CONTEXT_EXTERNAL_PREFIX_LOCATION}/src/target-theora ${CONTEXT_MAKE} install
                    )

externalproject_add(target-cegui
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-zlib target-freetype target-ogre1
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/roninest/cegui-static.git
                    GIT_TAG v0-8-7${CEGUI_GIT_TAG}
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DCEGUI_BUILD_STATIC_CONFIGURATION=true
                    -DCEGUI_BUILD_SHARED_LIBS_WITH_STATIC_DEPENDENCIES=false
                    -DCEGUI_MSVC_STATIC_RUNTIME=false
                    -DOpenGL_GL_PREFERENCE=GLVND
                    -DCEGUI_INSTALL_WITH_RPATH=false
                    -DCEGUI_BUILD_PYTHON_MODULES=false
                    -DCEGUI_SAMPLES_ENABLED=false
                    -DCEGUI_BUILD_IMAGECODEC_DEVIL=false
                    -DCEGUI_BUILD_IMAGECODEC_FREEIMAGE=false
                    -DCEGUI_BUILD_IMAGECODEC_SDL2=false
                    -DCEGUI_BUILD_IMAGECODEC_STB=true
                    -DCEGUI_BUILD_IMAGECODEC_SILLY=false
                    -DCEGUI_BUILD_APPLICATION_TEMPLATES=false
                    -DCEGUI_BUILD_XMLPARSER_EXPAT=false
                    -DCEGUI_BUILD_XMLPARSER_XERCES=false
                    -DCEGUI_BUILD_XMLPARSER_TINYXML=false
                    -DCEGUI_BUILD_XMLPARSER_RAPIDXML=true
                    -DCEGUI_OPTION_DEFAULT_XMLPARSER=RapidXMLParser
                    -DCEGUI_HAS_PCRE_REGEX=false
                    -DCEGUI_STATIC_XMLPARSER_MODULE=true
                    -DCEGUI_STRING_CLASS=2
                    -DCEGUI_CUSTOM_ALLOCATORS=false
                    -DCEGUI_OPTION_DEFAULT_IMAGECODEC=STBImageCodec
                    -DCEGUI_STATIC_IMAGECODEC_MODULE=true
                    -DCEGUI_BUILD_STATIC_FACTORY_MODULE=true
                    -DCEGUI_BUILD_RENDERER_OGRE=true
                    -DOGRE_DIR=${CONTEXT_EXTERNAL_INSTALL_LOCATION}/lib/OGRE/cmake
                    -DCEGUI_FOUND_OGRE_VERSION_MAJOR=1
                    -DCEGUI_FOUND_OGRE_VERSION_MINOR=12
                    -DCEGUI_FOUND_OGRE_VERSION_PATCH=5
                    -DFreetype_DIR=${CONTEXT_EXTERNAL_INSTALL_LOCATION}/lib/cmake/freetype
                    -DCEGUI_BUILD_RENDERER_OPENGL=false
                    -DCEGUI_BUILD_RENDERER_OPENGL3=false
                    -DCEGUI_BUILD_RENDERER_OPENGLES=false
                    -DCEGUI_BUILD_RENDERER_DIRECT3D9=false
                    -DCEGUI_BUILD_RENDERER_DIRECT3D10=false
                    -DCEGUI_BUILD_RENDERER_DIRECT3D11=false
                    -DCEGUI_BIDI_SUPPORT=false
                    -DCEGUI_USE_FRIBIDI=false
                    -DCEGUI_USE_MINIBIDI=false
                    -DCEGUI_HAS_MINIZIP_RESOURCE_PROVIDER=false
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-procedural
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-ogre1
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/OGRECave/ogre-procedural.git
                    GIT_TAG b678da1dd9c7f643e4992e08f8ff46604d7aa209
                    GIT_SHALLOW ${EXTERNAL_GIT_SHALLOW}
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DOgreProcedural_BUILD_SAMPLES=false
                    -DOgreProcedural_BUILD_DOCS=false
                    -DOgreProcedural_STATIC=true
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )

externalproject_add(target-particles
                    EXCLUDE_FROM_ALL true
                    DEPENDS target-ogre1
                    PREFIX ${CONTEXT_EXTERNAL_PREFIX_LOCATION}
                    GIT_REPOSITORY https://github.com/OGRECave/particleuniverse.git
                    GIT_TAG 886d992a08089bfb9f9a2b9a9b14722081133859
                    GIT_SHALLOW false
                    GIT_PROGRESS ${EXTERNAL_GIT_PROGRESS}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_PREFIX_PATH=${CONTEXT_EXTERNAL_INSTALL_LOCATION}
                    -DCMAKE_BUILD_TYPE=${EXTERNAL_BUILD_TYPE}
                    -DBUILD_SHARED_LIBS=false
                    -DCMAKE_CXX_FLAGS=${CONTEXT_EXTERNAL_CXX_FLAGS}
                    -DCMAKE_C_FLAGS=${CONTEXT_EXTERNAL_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${CONTEXT_EXTERNAL_EXE_LINKER_FLAGS}
                    -DCMAKE_STATIC_LINKER_FLAGS=${CONTEXT_EXTERNAL_STATIC_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${CONTEXT_EXTERNAL_SHARED_LINKER_FLAGS}
                    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                    ${CONTEXT_CMAKE_EXTRA_FLAGS}
                    -G "${CMAKE_GENERATOR}"
                    )
