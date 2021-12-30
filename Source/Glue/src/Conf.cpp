// This source file is part of "glue project". Created by Andrew Vasiliev

#include "PCHeader.h"
#include "Conf.h"
#include "Exception.h"
#include <fstream>
#include <inih/INIReader.h>

using namespace std;

namespace Glue {

Conf::Conf(const std::string &FileName) { Reload(FileName); }

Conf::~Conf() {}

void Conf::Reload(const std::string &FileName) {
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

#ifdef DESKTOP

std::string Conf::Get(std::string name, std::string default_value) const { return Document->Get(DefaultSection, name, default_value); }

std::string Conf::GetString(std::string name, std::string default_value) const { return Document->Get(DefaultSection, name, default_value); }

long Conf::GetInt(std::string name, long default_value) const { return Document->GetInteger(DefaultSection, name, default_value); }

double Conf::GetReal(std::string name, double default_value) const { return Document->GetReal(DefaultSection, name, default_value); }

float Conf::GetFloat(std::string name, float default_value) const { return Document->GetFloat(DefaultSection, name, default_value); }

bool Conf::GetBool(std::string name, bool default_value) const { return Document->GetBoolean(DefaultSection, name, default_value); }

#else

std::string Conf::Get(std::string name, std::string default_value) const { return default_value; }

std::string Conf::GetString(std::string name, std::string default_value) const { return default_value; }

long Conf::GetInt(std::string name, long default_value) const { return default_value; }

double Conf::GetReal(std::string name, double default_value) const { return default_value; }

float Conf::GetFloat(std::string name, float default_value) const { return default_value; }

bool Conf::GetBool(std::string name, bool default_value) const { return default_value; }


#endif

}  // namespace Glue
