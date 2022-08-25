// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once

namespace Glue {

/// Deletes copy constructor
class NoCopy {
 public:
  NoCopy() = default;
  NoCopy(const NoCopy &) = delete;
  NoCopy &operator=(const NoCopy &) = delete;
  NoCopy(NoCopy &&) = default;
  NoCopy &operator=(NoCopy &&) = default;
  virtual ~NoCopy() = default;
};

}  // namespace Glue
