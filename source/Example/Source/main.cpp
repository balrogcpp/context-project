/// created by Andrey Vasiliev

#include "pch.h"
#include "Application.h"

using namespace Glue;

class MenuAppState : public AppState {
 public:
  std::string GetName() { return "MenuAppState"; }
  void OnSetUp() {
    GetComponent<VideoManager>().GetWindow(0).GrabCursor(true);
    GetComponent<SceneManager>().LoadFromFile("1.scene");
    GetComponent<SkyManager>().SetUpSky();
    GetComponent<CompositorManager>().SetCompositorEnabled("Blur", true);
    GetComponent<CompositorManager>().SetCompositorEnabled("Bloom", true);
    GetComponent<CompositorManager>().SetCompositorEnabled("FXAA", true);
  }
  void OnUpdate(float time) {}
  void OnClean() {
    GetComponent<VideoManager>().ClearScene();
  }
};

#if defined(_WIN32) && !defined(DEBUG)
#include <windows.h>
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main()
#endif
{
  try {
    // main app class object
    Glue::Application app;

    GetComponent<AppStateManager>().RegAppState(std::make_shared<MenuAppState>());
    GetComponent<AppStateManager>().SetActiveAppState("MenuAppState");
//    GetComponent<VideoManager>().GetMainWindow().SetFullscreen(true);
    GetComponent<VideoManager>().ShowWindow(0, true);

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
