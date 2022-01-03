// This source file is part of "glue project". Created by Andrew Vasiliev

#include "PCHeader.h"
#include "Input/InputObserver.h"
#include "Input/InputSequencer.h"

using namespace std;

namespace Glue {

InputObserver::InputObserver() {}

InputObserver::~InputObserver() {}

void InputObserver::RegMyself() {
  static auto &ref = InputSequencer::GetInstance();
  ref.RegObserver(this);
}

void InputObserver::UnRegMyself() {
  static auto &ref = InputSequencer::GetInstance();
  ref.UnregObserver(this);
}

}  // namespace Glue
