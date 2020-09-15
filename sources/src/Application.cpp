/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "pcheader.h"

#include "Application.h"
#include "Storage.h"
#include "Exception.h"
#include "Overlay.h"

#ifdef _WIN32
extern "C"
{
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace xio {
Application::Application() {
  try {
    Init_();
  }
  catch (Exception &e) {
    Message_("Exception occurred", e.getDescription());
    throw e;
  }
  catch (Ogre::Exception &e) {
    Message_("Exception occurred (OGRE)", e.getFullDescription());
    throw e;
  }
  catch (std::exception &e) {
    Message_("Exception occurred (std::exception)", e.what());
    throw e;
  }
};
Application::~Application() = default;
//----------------------------------------------------------------------------------------------------------------------
void Application::Init_() {
  conf_ = std::make_unique<YamlConfigurator>("config.yaml");
  io_ = std::make_unique<InputSequencer>();
  renderer_ = std::make_unique<Renderer>();
  physics_ = std::make_unique<Physics>();
  sounds_ = std::make_unique<Sound>();
  overlay_ = std::make_unique<Overlay>();
  loader_ = std::make_unique<DotSceneLoaderB>();

  components_.push_back(loader_.get());
  components_.push_back(physics_.get());
  components_.push_back(sounds_.get());
  components_.push_back(renderer_.get());
  components_.push_back(overlay_.get());

  for (auto &it : components_)
    it->Create();

  io_->RegObserver(overlay_->GetConsole());
  renderer_->Refresh();

  // Texture filtering
  std::string graphics_filtration = conf_->Get<std::string>("graphics_filtration");
  Ogre::TextureFilterOptions tfo = Ogre::TFO_BILINEAR;
  if (graphics_filtration == "anisotropic")
    tfo = Ogre::TFO_ANISOTROPIC;
  else if (graphics_filtration == "bilinear")
    tfo = Ogre::TFO_BILINEAR;
  else if (graphics_filtration == "trilinear")
    tfo = Ogre::TFO_TRILINEAR;
  else if (graphics_filtration == "none")
    tfo = Ogre::TFO_NONE;

  renderer_->UpdateParams(tfo, conf_->Get<int>("graphics_anisotropy_level"));

  bool shadow_enable = conf_->Get<bool>("graphics_shadows_enable");
  float shadow_far = conf_->Get<float>("graphics_shadows_far_distance");
  int16_t tex_size = conf_->Get<int16_t>("graphics_shadows_texture_resolution");
  std::string tex_format_str = conf_->Get<std::string>("graphics_shadows_texture_format");
  Ogre::PixelFormat tex_format = Ogre::PF_DEPTH16;

  if (tex_format_str == "F32_R")
    tex_format = Ogre::PF_FLOAT32_R;
  else if (tex_format_str == "F16_R")
    tex_format = Ogre::PF_FLOAT16_R;
  else if (tex_format_str == "F32_GR")
    tex_format = Ogre::PF_FLOAT32_GR;
  else if (tex_format_str == "F16_GR")
    tex_format = Ogre::PF_FLOAT16_GR;
  else if (tex_format_str == "DEPTH16")
    tex_format = Ogre::PF_DEPTH16;
  else if (tex_format_str == "DEPTH32")
    tex_format = Ogre::PF_DEPTH32;
  else if (tex_format_str == "DEPTH32F")
    tex_format = Ogre::PF_DEPTH32F;

  renderer_->GetShadowSettings()->UpdateParams(shadow_enable, shadow_far, tex_size, tex_format);

  loader_->LocateComponents(conf_.get(),io_.get(),renderer_.get(),physics_.get(),sounds_.get(),overlay_.get());
  verbose_ = conf_->Get<bool>("global_verbose_enable");
  lock_fps_ = conf_->Get<bool>("global_lock_fps");
  target_fps_ = conf_->Get<int>("global_target_fps");
  io_->RegWinObserver(this);
  renderer_->Resize(conf_->Get<int>("window_width"),conf_->Get<int>("window_high"), conf_->Get<bool>("window_fullscreen"));
  renderer_->GetWindow().SetCaption(conf_->Get<std::string>("window_caption"));

  if (!verbose_) {
    auto *logger = new Ogre::LogManager();
    std::string log_name = "Ogre.log";

    logger->createLog(log_name, true, false, true);
    Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);
    Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Clear_() {
  io_->UnregWinObserver(this);
  for (auto &it : components_)
    it->Clear();
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}
//----------------------------------------------------------------------------------------------------------------------
void Application::InitCurrState_() {
  cur_state_->LocateComponents(conf_.get(),io_.get(),renderer_.get(),physics_.get(),sounds_.get(),overlay_.get(),loader_.get());
  cur_state_->Create();
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Quit() {
  quit_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Event(const SDL_Event &evt) {
  if (evt.type == SDL_WINDOWEVENT) {
    static bool fullscreen = renderer_->GetWindow().GetFullscreen();

    if (!fullscreen) {
      if (evt.window.event == SDL_WINDOWEVENT_LEAVE || evt.window.event == SDL_WINDOWEVENT_MINIMIZED) {
        suspend_ = true;
        lock_fps_ = true;
      } else if (evt.window.event == SDL_WINDOWEVENT_TAKE_FOCUS || evt.window.event == SDL_WINDOWEVENT_RESTORED
          || evt.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        suspend_ = false;
      }
    } else {
      if (evt.window.event == SDL_WINDOWEVENT_MINIMIZED) {
        suspend_ = true;
        lock_fps_ = true;
      } else if (evt.window.event == SDL_WINDOWEVENT_TAKE_FOCUS || evt.window.event == SDL_WINDOWEVENT_RESTORED
          || evt.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        suspend_ = false;
      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Other(Uint8 type, int32_t code, void *data1, void *data2) {}
//----------------------------------------------------------------------------------------------------------------------
void Application::Loop_() {
  while (quit_) {
    if (cur_state_) {
      auto duration_before_frame = std::chrono::system_clock::now().time_since_epoch();
      long millis_before_frame = std::chrono::duration_cast<std::chrono::microseconds>(duration_before_frame).count();

      int fps_frames_ = 0;
      long delta_time_ = 0;

      if (delta_time_ > 1000000) {
        current_fps_ = fps_frames_;
        delta_time_ = 0;
        fps_frames_ = 0;
      }

      io_->Capture();

      if (!suspend_) {
        cur_state_->Loop();
        if (cur_state_->Waiting()) {
          for (auto &it : components_)
            it->Clean();

          cur_state_->Clear();
          renderer_->Refresh();

          Ogre::Root::getSingleton().removeFrameListener(cur_state_.get());
          io_->UnregObserver(cur_state_.get());
          cur_state_ = move(cur_state_->GetNextState());
          io_->RegObserver(cur_state_.get());
          Ogre::Root::getSingleton().addFrameListener(cur_state_.get());

          InitCurrState_();

          physics_->Start();
        }

        auto duration_before_update = std::chrono::system_clock::now().time_since_epoch();
        long millis_before_update = std::chrono::duration_cast<std::chrono::microseconds>(duration_before_update).count();
        float frame_time = static_cast<float>(millis_before_update - time_of_last_frame_) / 1000000;
        time_of_last_frame_ = millis_before_update;

        for (auto *it : components_)
          it->Loop(frame_time);

        renderer_->RenderOneFrame();
      }

#ifdef DEBUG
      if (verbose_)
        std::cout << std::flush;
#endif

      auto duration_after_render = std::chrono::system_clock::now().time_since_epoch();
      long millis_after_render = std::chrono::duration_cast<std::chrono::microseconds>(duration_after_render).count();
      long render_time = millis_after_render - millis_before_frame;

      if (lock_fps_) {
        long delay = static_cast<long> ((1000000 / target_fps_) - render_time);
        if (delay > 0)
          std::this_thread::sleep_for(std::chrono::microseconds(delay));
      }

      auto duration_after_loop = std::chrono::system_clock::now().time_since_epoch();
      long millis_after_loop = std::chrono::duration_cast<std::chrono::microseconds>(duration_after_loop).count();

      long time_since_last_frame_ = millis_after_loop - millis_before_frame;
      delta_time_ += time_since_last_frame_;

      fps_frames_++;
    } else {
      quit_ = true;
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Go_() {
  if (cur_state_) {
    InitCurrState_();
    quit_ = true;
    auto duration_before_update = std::chrono::system_clock::now().time_since_epoch();
    time_of_last_frame_ = std::chrono::duration_cast<std::chrono::microseconds>(duration_before_update).count();
    Loop_();
    io_->Clear();
    Clear_();
  }
}
//----------------------------------------------------------------------------------------------------------------------
int Application::Message_(const std::string &caption, const std::string &message) {
  std::cerr << caption << '\n';
  std::cerr << message << '\n';
  return 1;
}
//----------------------------------------------------------------------------------------------------------------------
int Application::Main(std::unique_ptr<AppState> &&scene_ptr) {
  try {
    std::ios_base::sync_with_stdio(false);
    SetInitialState(move(scene_ptr));
    Go_();
  }
  catch (Exception &e) {
    return Message_("Exception occurred", e.getDescription());
  }
  catch (Ogre::Exception &e) {
    return Message_("Exception occurred (OGRE)", e.getFullDescription());
  }
  catch (std::exception &e) {
    return Message_("Exception occurred (std::exception)", e.what());
  }

  return 0;
}
}