// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Application.h"

using namespace std;
using namespace Glue;

extern AppStateUPtr MainAppState();

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main(int argc, char *args[])
#endif
{
  try {
    // main app class object
#ifndef _WIN32
    Application app(argc, args);
#else
    Application app;
#endif
    // main function
    return app.Main(MainAppState());
  } catch (Exception &e) {
    ErrorWindow("Exception", e.GetDescription());
  } catch (Ogre::Exception &e) {
    ErrorWindow("Exception", e.getFullDescription());
  } catch (exception &e) {
    ErrorWindow("Exception", e.what());
  } catch (...) {
    ErrorWindow("Exception", "Unknown exception");
  }

  // to be sure app will return something to system
  return 0;
}
