// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Component.h"
#include "Engine.h"

using namespace std;

namespace Glue {

Component::Component() { Engine::GetInstance().RegSystem(this); }

Component::~Component() {}

}  // namespace Glue
