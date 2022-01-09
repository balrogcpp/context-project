// This source file is part of "glue project". Created by Andrey Vasiliev

#include "Components/ComponentImpl.h"

namespace Glue {

void ComponentImpl::OnPause() {
  Paused = true;
  // TODO fill this
}

void ComponentImpl::OnResume() {
  Paused = false;
  // TODO fill this
}

void ComponentImpl::OnUpdate(float TimePassed) {
  // TODO fill this
}

void ComponentImpl::OnClean() {
  // TODO fill this
}

}  // namespace Glue
