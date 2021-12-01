#pragma once
#include "ObjectPtr.h"
#include <memory>

namespace glue {

template <typename T>
using ViewPtr = jss::object_ptr<T>;

}
