/// created by Andrey Vasiliev

#include "pch.h"
#include "Application.h"

using namespace Glue;

class MenuAppState : public AppState {
 public:
  std::string GetName() { return "MenuAppState"; }
  void OnSetUp() {
    GetComponent<VideoManager>().GetWindow(0).SetMouseRelativeMode(true);
    // GetComponent<VideoManager>().GetWindow(0).SetFullscreen(true);
#ifdef ANDROID
    GetComponent<CompositorManager>().SetFixedViewportSize(1024, 768);
#endif
    GetComponent<SceneManager>().LoadFromFile("1.scene");
    GetComponent<SkyManager>().SetUpSky();
    GetComponent<CompositorManager>().SetCompositorEnabled("Blur", true);
  }
  void OnUpdate(float time) {
    static ImGuiIO &io = ImGui::GetIO();
    Ogre::ImGuiOverlay::NewFrame();
    static bool ShowContextMenu = false;
    // fps counter
    ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.5);
    ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    if (ShowContextMenu) {
      static bool use_work_area = true;
      static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
      const auto *viewport = ImGui::GetMainViewport();
      // ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
      // ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);
      ImGui::SetNextWindowBgAlpha(0.8);
      ImGui::Begin("Example: Fullscreen window", nullptr, flags);
      ImGui::End();
      GetComponent<PhysicsManager>().SetSleep(true);
      GetComponent<SceneManager>().SetSleep(true);
    } else {
      GetComponent<PhysicsManager>().SetSleep(false);
      GetComponent<SceneManager>().SetSleep(false);
      return;
    }

    // ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.5), ImGuiCond_Always, ImVec2(0.5, 0.5));
    // ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
    // ImGui::SetNextWindowBgAlpha(0.5);
    ImGui::SetNextWindowFocus();
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

    ImGui::NewLine();
    if (ImGui::Button(u8"    Back to game")) {
      ShowContextMenu = false;
    }
    ImGui::End();
    //    ImGui::NewLine();
    //    if (ImGui::Button(u8"    Main Menu")) {
    //      //ChangeState(make_unique<MenuAppState>());
    //    }
    //    ImGui::NewLine();
    //    if (ImGui::Button(u8"    Quit game")) {
    //      //ChangeState();
    //    }
  }
  void OnClean() { GetComponent<SystemLocator>().OnClean(); }
};

#if defined(_WIN32) && (defined(NDEBUG))
#include <windows.h>
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main()
#endif
{
  try {
    // main app class object
    Glue::Application app;
    app.Init();

    GetComponent<VideoManager>().ShowWindow(0, true);
    GetComponent<AppStateManager>().RegAppState(std::make_shared<MenuAppState>());
    GetComponent<AppStateManager>().SetActiveAppState("MenuAppState");

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
