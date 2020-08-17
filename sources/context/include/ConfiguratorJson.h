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

#pragma once

#include "ContextConfigStructures.h"
#include "Exception.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <filesystem>
#include <string>
#include <iostream>

namespace Context {

class ConfiguratorJson {
 public:
//----------------------------------------------------------------------------------------------------------------------
  static ConfiguratorJson &GetSingleton() {
    static ConfiguratorJson ConfigManagerSingleton;
    return ConfigManagerSingleton;
  }
//----------------------------------------------------------------------------------------------------------------------
  void Setup() {
#ifndef __ANDROID__
    if (!std::filesystem::exists(file_name_)) {
      file_name_ = std::string("../") + file_name_;

      if (!std::filesystem::exists(file_name_)) {
        throw Exception("Error during parsing of " + file_name_ + " : file not exist");
      }
    }
#else
    file_name_ = std::string("../") + file_name_;
#endif
    std::ifstream ifs(file_name_);

    rapidjson::IStreamWrapper isw(ifs);

    document_.ParseStream(isw);

    if (ifs.is_open()) {
      ifs.close();
    } else {
      throw Exception("Error during parsing of " + file_name_ + " : can't open file");
    }

    if (!document_.IsObject()) {
      throw Exception("Error during parsing of " + file_name_ + " : file is empty or incorrect");
    }

    auto &allocator = document_.GetAllocator();

    document_.AddMember("window_caption", "Demo Application", allocator);
    document_.AddMember("window_width", 1920, allocator);
    document_.AddMember("window_high", 1080, allocator);
    document_.AddMember("window_fullscreen", false, allocator);

    document_.AddMember("global_octree_enable", true, allocator);
    document_.AddMember("global_stbi_enable", true, allocator);
    document_.AddMember("global_particlefx_enable", true, allocator);
    document_.AddMember("global_sso_enable", true, allocator);

    document_.AddMember("physics_enable", true, allocator);
    document_.AddMember("sound_enable", true, allocator);

    document_.AddMember("opengl_ver_force", false, allocator);
    document_.AddMember("opengl_ver_major", 3, allocator);
    document_.AddMember("opengl_ver_minor", 3, allocator);

    document_.AddMember("rtss_enable", true, allocator);
    document_.AddMember("rtss_pssm4_enable", true, allocator);
    document_.AddMember("rtss_cache_enable", false, allocator);
    document_.AddMember("rtss_resolver_enable", false, allocator);
    document_.AddMember("rtss_perpixel_light_enable", false, allocator);
    document_.AddMember("rtss_perpixel_fog_enable", true, allocator);
    document_.AddMember("rtss_cache_dir", ".rtss", allocator);

    document_.AddMember("graphics_shadows_enable", true, allocator);
    document_.AddMember("compositor_use_bloom", true, allocator);
    document_.AddMember("compositor_use_ssao", false, allocator);
    document_.AddMember("compositor_use_blur", true, allocator);
    document_.AddMember("compositor_use_hdr", false, allocator);

    document_.AddMember("media_location_directory", "../", allocator);
    document_.AddMember("opengl_ver_force", false, allocator);
    document_.AddMember("opengl_ver_major", 3, allocator);
    document_.AddMember("opengl_ver_minor", 3, allocator);
    document_.AddMember("global_target_fps", 60, allocator);
    document_.AddMember("global_lock_fps", true, allocator);
    document_.AddMember("graphics_vsync", true, allocator);
    document_.AddMember("graphics_shadows_enable", true, allocator);

    document_.AddMember("graphics_fsaa", 0, allocator);
    document_.AddMember("graphics_debug_show_wireframe", false, allocator);
    document_.AddMember("graphics_gamma", false, allocator);
    document_.AddMember("graphics_filtration", "bilinear", allocator);
    document_.AddMember("graphics_anisotropy_level", 4, allocator);
    document_.AddMember("graphics_mipmap_count", 16, allocator);
    document_.AddMember("graphics_shadows_pssm_0_resolution", 1024, allocator);
    document_.AddMember("graphics_shadows_pssm_1_resolution", 1024, allocator);
    document_.AddMember("graphics_shadows_pssm_2_resolution", 1024, allocator);
    document_.AddMember("graphics_shadows_texture_resolution", 1024, allocator);
    document_.AddMember("graphics_shadows_split_auto", true, allocator);
    document_.AddMember("graphics_shadows_split_padding", 1.0, allocator);
    document_.AddMember("graphics_shadows_self_shadow", true, allocator);
    document_.AddMember("graphics_shadows_back_faces", true, allocator);
    document_.AddMember("graphics_shadows_caster_material", "PSSM/shadow_caster", allocator);
    document_.AddMember("graphics_shadows_receiver_material", std::string(), allocator);
    document_.AddMember("graphics_shadows_far_distance", 500.0f, allocator);
    document_.AddMember("graphics_shadows_pssm_0", 0.0f, allocator);
    document_.AddMember("graphics_shadows_pssm_1", 0.0f, allocator);
    document_.AddMember("graphics_shadows_pssm_2", 0.0f, allocator);
    document_.AddMember("graphics_shadows_texture_count", 3, allocator);
    document_.AddMember("graphics_shadows_tecnique", "texture", allocator);
    document_.AddMember("graphics_shadows_lighting", "additive", allocator);
    document_.AddMember("graphics_shadows_projection", "pssm", allocator);
    document_.AddMember("graphics_shadows_projection", "default", allocator);
    document_.AddMember("graphics_shadows_integrated", true, allocator);
    document_.AddMember("graphics_shadows_texture_format", "DEPTH32", allocator);
    document_.AddMember("terrain_fog_perpixel", true, allocator);
    document_.AddMember("terrain_receive_shadows", true, allocator);
    document_.AddMember("terrain_receive_shadows_low_lod", false, allocator);
    document_.AddMember("terrain_cast_shadows", false, allocator);
    document_.AddMember("terrain_lightmap_enable", false, allocator);
    document_.AddMember("terrain_lightmap_size", 4096, allocator);
    document_.AddMember("terrain_colourmap_enable", false, allocator);
    document_.AddMember("terrain_parallaxmap_enable", true, allocator);
    document_.AddMember("terrain_raybox_calculation", true, allocator);
    document_.AddMember("terrain_specularmap_enable", true, allocator);
    document_.AddMember("terrain_normalmap_enable", true, allocator);
    document_.AddMember("terrain_save_terrains", false, allocator);
    document_.AddMember("lod_generator_enable", true, allocator);

    if (GetBool("global_verbose_enable")) {

      std::cout << "Global config variables:\n";
      std::cout << "--------------------------------------------\n";

      int counter = 0;
      for (auto it = document_.Begin(); it != document_.End(); ++it) {
        if (counter == 0) {
          std::cout << "variable: ";
        }

        const char true_string[] = "True";
        const char false_string[] = "False";

        if (it->IsString()) {
          std::cout << it->GetString();
        } else if (it->IsBool()) {
          std::cout << (it->GetBool() ? true_string : false_string);
        } else if (it->IsInt()) {
          std::cout << it->GetInt();
        } else if (it->IsInt64()) {
          std::cout << it->GetInt64();
        } else if (it->IsFloat()) {
          std::cout << it->GetFloat();
        } else if (it->IsDouble()) {
          std::cout << it->GetDouble();
        }

        if (counter == 0) {
          std::cout << " | value: ";
          counter++;
        } else if (counter == 1) {
          std::cout << '\n';
          counter = 0;
        }
      }

      std::cout << '\n';
    }

    if (ifs.is_open())
      ifs.close();
  }
//----------------------------------------------------------------------------------------------------------------------
  std::string GetConfigFileName() const {
    return file_name_;
  }
//----------------------------------------------------------------------------------------------------------------------
  void SetConfigFileName(const std::string &config_file_name) {
    file_name_ = config_file_name;
  }
//----------------------------------------------------------------------------------------------------------------------
  bool HasMember(const char *parameter) {
    return document_.HasMember(parameter);
  }
//----------------------------------------------------------------------------------------------------------------------
  int GetInt(const char *parameter) {
    if (document_[parameter].IsInt()) {
      return document_[parameter].GetInt();
    } else {
      return 0;
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  bool GetBool(const char *parameter) {
    if (document_[parameter].IsBool()) {
      return document_[parameter].GetBool();
    } else {
      return false;
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  std::string GetString(const char *parameter) {
    if (document_[parameter].IsString()) {
      return document_[parameter].GetString();
    } else {
      return std::string();
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  float GetFloat(const char *parameter) {
    if (document_[parameter].IsFloat()) {
      return document_[parameter].GetFloat();
    } else {
      return 0;
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  double GetDouble(const char *parameter) {
    if (document_[parameter].IsDouble()) {
      return document_[parameter].GetDouble();
    } else {
      return 0;
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(bool &value, const char *parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (GetSingleton().HasMember(parameter))
      value = GetSingleton().GetBool(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(int &value, const char *parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetInt(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(std::string &value, const char *parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetString(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(float &value, const char *parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetFloat(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(double &value, const char *parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetDouble(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  void Add(const char *name, T &&value) {
    if (!document_.HasMember(name))
      document_.AddMember(static_cast<rapidjson::GenericStringRef<char>>(name), value, document_.GetAllocator());
  }

 private:
  std::string file_name_ = "config.json";
  rapidjson::Document document_;
}; //class ConfigManager
} //namespace Context
