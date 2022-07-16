// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "WindowObserver.h"
#include "InputSequencer.h"

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
