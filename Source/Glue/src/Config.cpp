// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Config.h"
#include "AssetLoader.h"

using namespace std;

namespace Glue {

Config::Config(const string &FileName) {
  if (FileName.empty()) return;
  Reload(FileName);
}

Config::~Config() {}

void Config::Reload(const string &FileName) {
  Assert(!FileName.empty(), "Provided conf file name is empty");
  if(Document.size()) Document.clear();
  ifstream ifs(AssetLoader::FindPath(FileName));

  if (!ifs.is_open()) {
    Throw("Error during parsing of " + FileName + " : can't open file");
  }

  ifs >> Document;
}

}  // namespace Glue
