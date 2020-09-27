//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
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

#pragma once
#include "NoCopy.h"
#include <map>
#include <string>
#include <regex>
#include <filesystem>
#include <fstream>

namespace xio {
class DesktopIcon : public NoCopy {
 public:
  void Init() {
    run_dir_ = std::filesystem::current_path().string();
    skeleton = std::regex_replace(skeleton, std::regex("EXEC"), run_dir_ + "/" + exec_);
    skeleton = std::regex_replace(skeleton, std::regex("PATH"), run_dir_);
    skeleton = std::regex_replace(skeleton, std::regex("ICON"), icon_);
    skeleton = std::regex_replace(skeleton, std::regex("NAME"), name_);
    skeleton = std::regex_replace(skeleton, std::regex("COMMENT"), name_);
    skeleton = std::regex_replace(skeleton, std::regex("VERSION"), version);
  }

  void Save(const std::string &icon_name) {
    std::string home_dir = std::string(getenv("HOME"));
    std::string path = home_dir + "/.local/share/applications/" + icon_name + ".desktop";

    std::ifstream in;
    in.open(path);
    if (in.is_open()) {
      std::string s;
      getline(in, s, '\0');
      in.close();

      if (s == skeleton)
        return;
    }

    std::ofstream out;
    out.open(path);
    if (out.is_open()) {
      out << skeleton;
      out.close();
    }
  }

 private:
  std::string run_dir_;
  std::string exec_ = "demo";
  std::string icon_;
  std::string version = "1.0";
  std::string name_ = "XioDemo";
  std::map<std::string, std::string> properties;
  std::string skeleton = "[Desktop Entry]\n"
                         "Version=VERSION\n"
                         "Name=NAME\n"
                         "Comment=COMMENT\n"
                         "TryExec=EXEC\n"
                         "Exec=EXEC\n"
                         "GenericName=NAME\n"
                         "Icon=ICON\n"
                         "Path=PATH\n"
                         "Terminal=false\n"
                         "Type=Application\n";
};
}