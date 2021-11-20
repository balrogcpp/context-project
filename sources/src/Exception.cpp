// This source file is part of "glue project"
// Created by Andrew Vasiliev

#include "pcheader.h"
#include "Engine.h"
#include "Config.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
void Assert(bool result, std::string message) {
#ifdef DEBUG
  if (!result) {
    throw Exception(std::move(message));
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Throw(std::string message) { throw Exception(std::move(message)); }

}  // namespace glue
