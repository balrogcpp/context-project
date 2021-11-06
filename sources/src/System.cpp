// This source file is part of context-project
// Created by Andrew Vasiliev

#include "System.h"

#include "Engine.h"
#include "pcheader.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
System::System() { Engine::GetInstance().RegSystem(this); }

//----------------------------------------------------------------------------------------------------------------------
System::~System() {}

}  // namespace glue
