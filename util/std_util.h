#pragma once

#include <optional>

#include "util/macro_util.h"

#define RETURN_IF_NULL(expr) \
  do {                       \
    auto _opt = (expr);      \
    if (!_opt.has_value()) { \
      return std::nullopt;   \
    }                        \
  } while (0)

#define DEFINE_OR_RETURN_OPT_IMPL(type, lhs, tmp, ...) \
  std::optional<type> tmp = (__VA_ARGS__);             \
  if (!(tmp).has_value()) {                            \
    return std::nullopt;                               \
  }                                                    \
  type &lhs = (tmp).value()

#define DEFINE_OR_RETURN_OPT(type, lhs, ...)                              \
  DEFINE_OR_RETURN_OPT_IMPL(                                              \
      type, lhs, UTILS_CONCAT_NAME(__define_or_return_opt_, __COUNTER__), \
      __VA_ARGS__)
