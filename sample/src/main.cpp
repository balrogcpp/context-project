// This source file is part of "glue project"
// Created by Andrew Vasiliev

#include "Application.h"
#include "DemoDotAppState.h"
#include "MenuAppState.h"
#include "pch.h"

using namespace std;
using namespace glue;

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main(int argc, char* args[])
#endif
{
  // main app class object
  Application app;
  // main function
  return app.Main(make_unique<Demo::MenuAppState>());
  // to be sure app will return something to system
  return 0;
}
