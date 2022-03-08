// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include <OgrePlatform.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WINDOWS
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#define ANDROID
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#define APPLE
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#define IOS
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#define LINUX
#endif

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#define MSVC
#elif OGRE_COMPILER == OGRE_COMPILER_GNUC
#define GNUC
#elif OGRE_COMPILER == OGRE_COMPILER_CLANG
#define CLANG
#endif

#if defined(ANDROID) || defined(IOS)
#define MOBILE
#endif

#if defined(WINDOWS) || defined(APPLE) || defined(LINUX)
#define DESKTOP
#endif

#if defined(APPLE) || defined(LINUX)
#define UNIX
#endif

#if defined(__MINGW32__)
#define MINGW
#endif
