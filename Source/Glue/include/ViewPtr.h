#pragma once
#include "ObjectPtr.h"
#include <memory>

namespace Glue {

template <typename T>
using ViewPtr = jss::object_ptr<T>;

}
