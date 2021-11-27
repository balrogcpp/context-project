// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once

namespace glue {

class NoCopy {
 public:
  NoCopy() = default;
  NoCopy(const NoCopy &) = delete;
  NoCopy &operator=(const NoCopy &) = delete;
  virtual ~NoCopy() = default;
};

}  // namespace glue
