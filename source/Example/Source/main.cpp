/// created by Andrey Vasiliev

#include "pch.h"
#include "Application.h"

using namespace Glue;

#if defined(_WIN32) && !defined(DEBUG)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main()
#endif
{
  try {
    // main app class object
    Glue::Application app;

    auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
    auto *root = scene->getRootSceneNode();
    root->loadChildren("1.scene");
    GetComponent<VideoManager>().InitSky();
    //GetComponent<VideoManager>().GetWindow(0).GrabCursor(true);

    // main function
    return app.Main();
  } catch (std::exception &e) {
    return ErrorWindow("Exception", e.what());
  } catch (...) {
    return ErrorWindow("Exception", "unknown");
  }

  // to be sure app will return something to system
  return 0;
}
