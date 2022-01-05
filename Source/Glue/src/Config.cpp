// This source file is part of "glue project". Created by Andrew Vasiliev

#include "PCHeader.h"
#include "Config.h"
#include "Exception.h"
#include <fstream>
#include <inih/INIReader.h>

using namespace std;

namespace Glue {

Config::Config(const std::string &FileName) {
  Reload(FileName);
  ConfFileName = FileName;
}

Config::~Config() {}

void Config::Reload(const std::string &FileName) {
#ifdef DESKTOP
  if (FileName.empty()) return;
  std::ifstream ifs(FileName);
  if (!ifs.is_open()) {
    Throw(FileName + " does not exists");
  }

  Document.reset();
  Document = std::make_unique<INIReader>(FileName);
  if (Document->ParseError()) {
    Throw(FileName + std::to_string(Document->ParseError()));
  }
#endif
}

const string &Config::GetDefaultSection() const { return DefaultSection; }

void Config::SetDefaultSection(const string &defaultSection) { DefaultSection = defaultSection; }

#ifdef DESKTOP

std::string Config::Get(std::string name, std::string default_value) const { return Document->Get(DefaultSection, name, default_value); }

std::string Config::GetString(std::string name, std::string default_value) const { return Document->Get(DefaultSection, name, default_value); }

long Config::GetInt(std::string name, long default_value) const { return Document->GetInteger(DefaultSection, name, default_value); }

double Config::GetReal(std::string name, double default_value) const { return Document->GetReal(DefaultSection, name, default_value); }

float Config::GetFloat(std::string name, float default_value) const { return Document->GetFloat(DefaultSection, name, default_value); }

bool Config::GetBool(std::string name, bool default_value) const { return Document->GetBoolean(DefaultSection, name, default_value); }

#else

std::string Conf::Get(std::string name, std::string default_value) const { return default_value; }

std::string Conf::GetString(std::string name, std::string default_value) const { return default_value; }

long Conf::GetInt(std::string name, long default_value) const { return default_value; }

double Conf::GetReal(std::string name, double default_value) const { return default_value; }

float Conf::GetFloat(std::string name, float default_value) const { return default_value; }

bool Conf::GetBool(std::string name, bool default_value) const { return default_value; }

#endif

}  // namespace Glue
