// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Config.h"
#include "Exception.h"
#include <fstream>
#include "INIReader.h"

using namespace std;

namespace Glue {

#ifdef DESKTOP
const static string FILE_CONTENT =
    "[DEFAULT]\n"
#ifdef DEBUG
    "verbose=true\n"
#else
    "verbose=false\n"
#endif
    "verbose_input=false\n"
    "window_width=1024\n"
    "window_high=768\n"
    "window_fullscreen=false\n"
    "enable_bloom=false\n"
    "enable_ssao=false\n"
    "enable_mblur=false\n"
    "target_fps=30\n"
    "lock_fps=true\n"
    "vsync=true\n"
    "shadows_enable=true\n"
    "fsaa=0\n"
    "filtration=bilinear\n"
    "anisotropy_level=0\n"
    "tex_size=1024\n"
    "shadow_far=400\n";
#endif

Config::Config(const string &FileName) {
  Reload(FileName);
  ConfFileName = FileName;
}

Config::~Config() {}

void Config::Reload(const string &FileName) {
#ifdef DESKTOP
  if (FileName.empty()) return;

  ifstream ifs(FileName);
  if (!ifs.is_open()) {
    // File does not exists, create it
    ofstream ofs(FileName);
    if (ofs.is_open()) {
      ofs << FILE_CONTENT;
      ofs.close();
    } else {
      Throw(string("Can't open ") + FileName);
    }
  }

  Document.reset();
  Document = make_unique<INIReader>(FileName);
  if (Document->ParseError()) {
    Throw(FileName + to_string(Document->ParseError()));
  }
#endif
}

const string &Config::GetDefaultSection() const { return DefaultSection; }

void Config::SetDefaultSection(const string &defaultSection) { DefaultSection = defaultSection; }

#ifdef DESKTOP

string Config::Get(string name, string default_value) const { return Document->Get(DefaultSection, name, default_value); }

string Config::GetString(string name, string default_value) const { return Document->Get(DefaultSection, name, default_value); }

long Config::GetInt(string name, long default_value) const { return Document->GetInteger(DefaultSection, name, default_value); }

double Config::GetReal(string name, double default_value) const { return Document->GetReal(DefaultSection, name, default_value); }

float Config::GetFloat(string name, float default_value) const { return Document->GetFloat(DefaultSection, name, default_value); }

bool Config::GetBool(string name, bool default_value) const { return Document->GetBoolean(DefaultSection, name, default_value); }

#else

string Config::Get(string name, string default_value) const { return default_value; }

string Config::GetString(string name, string default_value) const { return default_value; }

long Config::GetInt(string name, long default_value) const { return default_value; }

double Config::GetReal(string name, double default_value) const { return default_value; }

float Config::GetFloat(string name, float default_value) const { return default_value; }

bool Config::GetBool(string name, bool default_value) const { return default_value; }

#endif

}  // namespace Glue
