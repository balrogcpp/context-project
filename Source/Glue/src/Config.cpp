// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Config.h"
#include "AssetLoader.h"

using namespace std;

namespace Glue {

Config::Config(const string &FileName) {
  if (!FileName.empty()) {
    Load(FileName);
  }
}

Config::~Config() {}

void Config::Load(const string &FileName) {
  ifstream ifs(AssetLoader::FindPath(FileName));

  if (!ifs.is_open()) {
    throw Exception("Error during parsing of " + FileName + " : can't open file");
  }

  ifs >> Document;
}

}  // namespace Glue
