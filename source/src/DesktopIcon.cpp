// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "DesktopIcon.h"
#if HAS_FILESYSTEM
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif
#include <regex>

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
DesktopIcon::DesktopIcon()
    : executable("sample"),
      version("1.0"),
      icon_name("GlueSample"),
      skeleton{
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
void DesktopIcon::SetUp() {
  root_directory = fs::current_path().string();

  output = skeleton;
  output = regex_replace(output, regex("EXEC"), root_directory + "/" + executable);
  output = regex_replace(output, regex("PATH"), root_directory);
  output = regex_replace(output, regex("ICON"), icon);
  output = regex_replace(output, regex("NAME"), icon_name);
  output = regex_replace(output, regex("COMMENT"), icon_name);
  output = regex_replace(output, regex("VERSION"), version);
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

    if (s == output) return;
  }

  ofstream out;
  out.open(path);
  if (out.is_open()) {
    out << output;
  }
}

}  // namespace glue
