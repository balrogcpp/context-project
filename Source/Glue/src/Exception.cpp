// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Exception.h"
#include "Config.h"
#include "Engine.h"

using namespace std;

namespace Glue {

void Assert(bool Result, std::string Message) {
  if (!Result) {
    throw Exception(std::move(Message));
  }
}

void Throw(std::string Message) { throw Exception(std::move(Message)); }

}  // namespace Glue