// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Config.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
Config::Config(const string &file_name) {
  if (!file_name.empty()) {
    Load(file_name);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Config::~Config() {}

//----------------------------------------------------------------------------------------------------------------------
void Config::Load(const string &file_name) {
  ifstream ifs(file_name);

  if (!ifs.is_open()) {
    throw Exception("Error during parsing of " + file_name + " : can't open file");
  }

  ifs >> document;
}

}  // namespace glue
