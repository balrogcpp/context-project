// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/ComponentImpl.h"

namespace Glue {

ComponentImpl::ComponentImpl() {
  // Add your code
}

ComponentImpl::~ComponentImpl() {
  // Add your code
}

void ComponentImpl::OnPause() {
  Paused = true;
  // Add your code
}

void ComponentImpl::OnResume() {
  Paused = false;
  // Add your code
}

void ComponentImpl::OnUpdate(float TimePassed) {
  // Add your code
}

void ComponentImpl::OnClean() {
  // Add your code
}

}  // namespace Glue
