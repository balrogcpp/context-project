// This source file is part of "glue project". Created by Andrew Vasiliev

#include "PCHeader.h"
#include "Input/InputObserver.h"
#include "Input/InputSequencer.h"

using namespace std;

namespace Glue {

InputObserver::InputObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.RegObserver(this);
}

InputObserver::~InputObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.UnregObserver(this);
}

}  // namespace Glue
