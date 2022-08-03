// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Application.h"
#include "DemoDotAppState.h"
#include "MenuAppState.h"

using namespace std;
using namespace Glue;

AppStateUPtr MainAppState() {
  return make_unique<Demo::DemoDotAppState>();
}
