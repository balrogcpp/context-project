//MIT License
//
//Copyright (c) 2021 Andrew Vasilev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "DemoDotAppState.h"
#include "MenuAppState.h"
#include "DesktopApplication.h"

using namespace std;
using namespace xio;

#if defined WIN32 && defined WINAPI_MAIN_FUNC
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main(int argc, char* args[] )
#endif
{
  DesktopApplication app;
  return app.Main(make_unique<Demo::MenuAppState>());
}


/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL, standard IO, and, strings
//#include <SDL2/SDL.h>
//#include <stdio.h>
//#include <string>
//#include <string.h>
//
////Starts up SDL and creates window
//bool init();
//
//
////Frees media and shuts down SDL
//void close();
//
////The window we'll be rendering to
//SDL_Window* gWindow = NULL;
//
////The window renderer
//SDL_Renderer* gRenderer = NULL;
//
////Screen dimensions
//SDL_Rect gScreenRect = { 0, 0, 320, 240 };
//
//bool init()
//{
//  //Initialization flag
//  bool success = true;
//
//  //Initialize SDL
//  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
//  {
//	SDL_Log( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
//	success = false;
//  }
//  else
//  {
//	//Set texture filtering to linear
//	if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
//	{
//	  SDL_Log( "Warning: Linear texture filtering not enabled!" );
//	}
//
//	//Get device display mode
//	SDL_DisplayMode displayMode;
//	if( SDL_GetCurrentDisplayMode( 0, &displayMode ) == 0 )
//	{
//	  gScreenRect.w = displayMode.w;
//	  gScreenRect.h = displayMode.h;
//	}
//
//	//Create window
//	gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gScreenRect.w, gScreenRect.h, SDL_WINDOW_SHOWN );
//	if( gWindow == NULL )
//	{
//	  SDL_Log( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
//	  success = false;
//	}
//	else
//	{
//	  //Create renderer for window
//	  gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
//	  if( gRenderer == NULL )
//	  {
//		SDL_Log( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
//		success = false;
//	  }
//	  else
//	  {
//		//Initialize renderer color
//		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
//	  }
//	}
//  }
//
//  return success;
//}
//
//
//void close()
//{
//  //Destroy window
//  SDL_DestroyRenderer( gRenderer );
//  SDL_DestroyWindow( gWindow );
//  gWindow = NULL;
//  gRenderer = NULL;
//
//  //Quit SDL subsystems
//  SDL_Quit();
//}
//
//int main( int argc, char* args[] )
//{
//  //Start up SDL and create window
//  if( !init() )
//  {
//	SDL_Log( "Failed to initialize!\n" );
//  }
//  else
//  {
//	//Load media
//	if( false)
//	{
//	  SDL_Log( "Failed to load media!\n" );
//	}
//	else
//	{
//	  //Main loop flag
//	  bool quit = false;
//
//	  //Event handler
//	  SDL_Event e;
//
//	  //While application is running
//	  while( !quit )
//	  {
//		//Handle events on queue
//		while( SDL_PollEvent( &e ) != 0 )
//		{
//		  //User requests quit
//		  if( e.type == SDL_QUIT )
//		  {
//			quit = true;
//		  }
//		}
//
//		//Clear screen
//		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x0, 0x0, 0xFF );
//		SDL_RenderClear( gRenderer );
//		SDL_Delay(32);
//		//Update screen
//		SDL_RenderPresent( gRenderer );
//	  }
//	}
//  }
//
//  //Free resources and close SDL
//  close();
//
//  return 0;
//}