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

namespace xio {
class DesktopIcon : public NoCopy {
 public:
  void Init();
  void Save(const std::string &icon_name);

 private:
  std::string run_dir_;
  std::string exec_ = "demo";
  std::string icon_;
  std::string version = "1.0";
  std::string name_ = "XioDemo";
  std::map<std::string, std::string> properties;
  std::string output_;
  const std::string skeleton_ = "[Desktop Entry]\n"
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