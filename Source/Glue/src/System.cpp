// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "System.h"
#include "Engine.h"

using namespace std;

namespace Glue {

System::System() { Engine::GetInstance().RegSystem(this); }

System::~System() {}

}  // namespace glue
