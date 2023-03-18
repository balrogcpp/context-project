/// created by Andrey Vasiliev

#include "pch.h"
#include "Application.h"
#include "Game.h"
#include "Menu.h"

using namespace gge;

#if defined(_WIN32) && defined(NDEBUG)
#include <windows.h>
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main()
#endif
{
  try {
    // main app class object
    gge::Application app;
    app.Init();

    GetComponent<VideoManager>().ShowWindow(true);
    GetComponent<AppStateManager>().RegAppState(std::make_shared<Menu>());
    GetComponent<AppStateManager>().SetActiveAppState("Menu");

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
