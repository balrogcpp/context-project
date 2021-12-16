// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Application.h"
#include "DemoDotAppState.h"
#include "MenuAppState.h"

using namespace std;
using namespace Glue;

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
  return app.Main(make_unique<Demo::DemoDotAppState>());
  // to be sure app will return something to system
  return 0;
}
