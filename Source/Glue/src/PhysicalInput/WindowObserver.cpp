// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "PhysicalInput/WindowObserver.h"
#include "PhysicalInput/InputSequencer.h"

using namespace std;

namespace Glue {

WindowObserver::WindowObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.RegWinObserver(this);
}

WindowObserver::~WindowObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.UnregWinObserver(this);
}

}  // namespace Glue