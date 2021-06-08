// This source file is part of context-project
// Created by Andrew Vasiliev

#include "DesktopIcon.h"

#include <regex>

#include "pcheader.h"
#if HAS_FILESYSTEM
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
DesktopIcon::DesktopIcon()
    : exec_("demo"),
      version_("1.0"),
      name_("XioDemo"),
      skeleton_{
          "[Desktop Entry]\n"
          "Version=VERSION\n"
          "Name=NAME\n"
          "Comment=COMMENT\n"
          "TryExec=EXEC\n"
          "Exec=EXEC\n"
          "GenericName=NAME\n"
          "Icon=ICON\n"
          "Path=PATH\n"
          "Terminal=false\n"
          "Type=Application\n"} {}

//----------------------------------------------------------------------------------------------------------------------
DesktopIcon::~DesktopIcon() = default;

//----------------------------------------------------------------------------------------------------------------------
void DesktopIcon::Init() {
  run_dir_ = fs::current_path().string();

  output_ = skeleton_;
  output_ = regex_replace(output_, regex("EXEC"), run_dir_ + "/" + exec_);
  output_ = regex_replace(output_, regex("PATH"), run_dir_);
  output_ = regex_replace(output_, regex("ICON"), icon_);
  output_ = regex_replace(output_, regex("NAME"), name_);
  output_ = regex_replace(output_, regex("COMMENT"), name_);
  output_ = regex_replace(output_, regex("VERSION"), version_);
}

//----------------------------------------------------------------------------------------------------------------------
void DesktopIcon::Save(const string &icon_name) {
  string home_dir = string(getenv("HOME"));
  string path = home_dir + "/.local/share/applications/" + icon_name + ".desktop";

  ifstream in;
  in.open(path);
  if (in.is_open()) {
    string s;
    getline(in, s, '\0');
    in.close();

    if (s == output_) return;
  }

  ofstream out;
  out.open(path);
  if (out.is_open()) {
    out << output_;
  }
}

}  // namespace xio
