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
#include "Application.h"

using namespace std;
using namespace xio;

#if defined WIN32 && defined NDEBUG
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main(int argc, char* args[])
#endif
{
  try {
	Application app;
	return app.Main(make_unique<Demo::DemoDotAppState>());
  } catch (...) {

  }
}


/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//#include "SDL2.hpp"
//
////Starts up SDL and creates window
//bool init();
//
////Frees media and shuts down SDL
//void close();
//
////The window we'll be rendering to
//static SDL_Window *gWindow = nullptr;
//
////The window renderer
//static SDL_Renderer *gRenderer = nullptr;
//
////Screen dimensions
//
//bool init() {
//  //Initialization flag
//  bool success = true;
//
//  //Initialize SDL
//  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_SENSOR) < 0) {
//	SDL_Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
//	success = false;
//  } else {
//	//Set texture filtering to linear
//
//	SDL_Rect gScreenRect = {0, 0, 320, 240};
//
//	//Get device display mode
//	SDL_DisplayMode displayMode;
//	if (SDL_GetCurrentDisplayMode(0, &displayMode)==0) {
//	  gScreenRect.w = displayMode.w;
//	  gScreenRect.h = displayMode.h;
//	}
//
//	//Create window
//	gWindow = SDL_CreateWindow("SDL Tutorial",
//							   SDL_WINDOWPOS_UNDEFINED,
//							   SDL_WINDOWPOS_UNDEFINED,
//							   gScreenRect.w,
//							   gScreenRect.h,
//							   SDL_WINDOW_SHOWN);
//
//	if (gWindow==nullptr) {
//	  SDL_Log("Window could not be created! SDL Error: %s\n", SDL_GetError());
//	  success = false;
//	} else {
//	  //Create renderer for window
//	  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
//	  if (gRenderer==nullptr) {
//		SDL_Log("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
//		success = false;
//	  } else {
//		//Initialize renderer color
//		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
//	  }
//	}
//  }
//
//  return success;
//}
//
//void close() {
//  //Destroy window
//  SDL_DestroyRenderer(gRenderer);
//  SDL_DestroyWindow(gWindow);
//  gWindow = nullptr;
//  gRenderer = nullptr;
//
//  //Quit SDL subsystems
//  SDL_Quit();
//}
//
//int main(int argc, char *args[]) {
//  if (!init()) {
//	SDL_Log("Failed to initialize!\n");
//  } else {
//	//Main loop flag
//	bool quit = false;
//
//	//Event handler
//	SDL_Event e;
//
//	//While application is running
//	while (!quit) {
//	  //Handle events on queue
//	  while (SDL_PollEvent(&e)!=0) {
//		//User requests quit
//		if (e.type==SDL_QUIT) {
//		  quit = true;
//		}
//	  }
//
//	  //Clear screen
//	  SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x0, 0x0, 0xFF);
//	  SDL_RenderClear(gRenderer);
//	  SDL_Delay(32);
//	  //Update screen
//	  SDL_RenderPresent(gRenderer);
//	}
//  }
//
//  //Free resources and close SDL
//  close();
//
//  return 0;
//}

//#include <SDL2.hpp>
//
//#define GL_GLEXT_PROTOTYPES 1
//#include <SDL2/SDL_opengles2.h>
//
//// Shader sources
//const GLchar* vertexSource =
//	"attribute vec4 position;    \n"
//	"void main()                  \n"
//	"{                            \n"
//	"   gl_Position = vec4(position.xyz, 1.0);  \n"
//	"}                            \n";
//const GLchar* fragmentSource =
//	"precision mediump float;\n"
//	"void main()                                  \n"
//	"{                                            \n"
//	"  gl_FragColor = vec4 (1.0, 1.0, 1.0, 1.0 );\n"
//	"}                                            \n";
//
//
//int main(int argc, char** argv)
//{
//  SDL_Init(SDL_INIT_VIDEO);
//
//  SDL_DisplayMode DM;
//  SDL_GetCurrentDisplayMode(0, &DM);
//
//  auto screen_w_ = static_cast<int>(DM.w);
//  auto screen_h_ = static_cast<int>(DM.h);
//
//  auto wnd(
//	  SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
//					   screen_w_, screen_h_, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN));
//
//  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
//  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//  SDL_GL_SetSwapInterval(0);
//  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
//
//  auto glc = SDL_GL_CreateContext(wnd);
//
//  auto rdr = SDL_CreateRenderer(
//	  wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
//
//  // Create Vertex Array Object
//  GLuint vao;
//  glGenVertexArraysOES(1, &vao);
//  glBindVertexArrayOES(vao);
//
//  // Create a Vertex Buffer Object and copy the vertex data to it
//  GLuint vbo;
//  glGenBuffers(1, &vbo);
//
//  GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};
//
//  glBindBuffer(GL_ARRAY_BUFFER, vbo);
//  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//  // Create and compile the vertex shader
//  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//  glShaderSource(vertexShader, 1, &vertexSource, NULL);
//  glCompileShader(vertexShader);
//
//  // Create and compile the fragment shader
//  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
//  glCompileShader(fragmentShader);
//
//  // Link the vertex and fragment shader into a shader program
//  GLuint shaderProgram = glCreateProgram();
//  glAttachShader(shaderProgram, vertexShader);
//  glAttachShader(shaderProgram, fragmentShader);
//  // glBindFragDataLocation(shaderProgram, 0, "outColor");
//  glLinkProgram(shaderProgram);
//  glUseProgram(shaderProgram);
//
//  // Specify the layout of the vertex data
//  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
//  glEnableVertexAttribArray(posAttrib);
//  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
//
//  bool quit = false;
//
//  while(!quit) {
//	SDL_Event e;
//	while(SDL_PollEvent(&e))
//	{
//	  if(e.type == SDL_QUIT) quit = true;
//	}
//
//	// Clear the screen to black
//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	// Draw a triangle from the 3 vertices
//	glDrawArrays(GL_TRIANGLES, 0, 3);
//
//	SDL_GL_SwapWindow(wnd);
//  };
//
//  return 0;
//}
