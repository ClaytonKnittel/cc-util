#pragma once

#include <cstdlib>
#include <iostream>

#ifdef UTIL_NDEBUG

#define UTIL_ASSERT(cond)

#else

#define UTIL_ASSERT(cond)                                                   \
  do {                                                                      \
    if (!(cond)) {                                                          \
      std::cerr << __FILE__ ":" << __LINE__ << ": Condition failed: " #cond \
                << std::endl;                                               \
      std::abort();                                                         \
    }                                                                       \
  } while (0)

#endif
