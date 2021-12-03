// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Config.h"
#include "Engine.h"

using namespace std;

namespace Glue {

void Assert(bool result, std::string message) {
#ifdef DEBUG
  if (!result) {
    throw Exception(std::move(message));
  }
#endif
}

void Throw(std::string message) { throw Exception(std::move(message)); }

}  // namespace glue
