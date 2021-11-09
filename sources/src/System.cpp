// This source file is part of "glue project"
// Created by Andrew Vasiliev

#include "pcheader.h"
#include "System.h"
#include "Engine.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
System::System() { Engine::GetInstance().RegSystem(this); }

//----------------------------------------------------------------------------------------------------------------------
System::~System() {}

}  // namespace glue
