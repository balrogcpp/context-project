// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#ifdef DESKTOP
#include "DesktopIcon.h"
#include "Filesystem.h"
#include <regex>

using namespace std;

namespace Glue {

DesktopIcon::DesktopIcon() : executable("Sample"), version("1.0"), icon_name("GlueSample") {
  skeleton =
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
      "Type=Application\n";

  root_directory = fs::current_path().string();

  output = skeleton;
  output = regex_replace(output, regex("EXEC"), root_directory + "/" + executable);
  output = regex_replace(output, regex("PATH"), root_directory);
  output = regex_replace(output, regex("ICON"), icon);
  output = regex_replace(output, regex("NAME"), icon_name);
  output = regex_replace(output, regex("COMMENT"), icon_name);
  output = regex_replace(output, regex("VERSION"), version);
}

DesktopIcon::~DesktopIcon() = default;

void DesktopIcon::SaveToFile(const string &icon_name) {
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

}  // namespace Glue

#endif
