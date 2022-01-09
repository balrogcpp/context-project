// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once
extern "C" {
#ifdef _MSC_VER
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
}
