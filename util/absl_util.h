#pragma once

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#define RETURN_IF_ERROR(expr)      \
  do {                             \
    absl::Status _status = (expr); \
    if (!_status.ok())             \
      return _status;              \
  } while (0)

// Internal helper for concatenating macro values.
#define UTILS_CONCAT_NAME_INNER(x, y) x##y
#define UTILS_CONCAT_NAME(x, y)       UTILS_CONCAT_NAME_INNER(x, y)

template <typename T>
absl::Status DoAssignOrReturn(T &lhs, absl::StatusOr<T> result) {
  if (result.ok()) {
    lhs = result.value();
  }
  return result.status();
}

#define ASSIGN_OR_RETURN_IMPL(status, lhs, ...)               \
  absl::Status status = DoAssignOrReturn(lhs, (__VA_ARGS__)); \
  if (!(status).ok())                                         \
    return status;

// Executes an expression that returns a util::StatusOr, extracting its value
// into the variable defined by lhs (or returning on error).
//
// Example: Assigning to an existing value
//   ValueType value;
//   ASSIGN_OR_RETURN(value, MaybeGetValue(arg));
//
// WARNING: ASSIGN_OR_RETURN expands into multiple statements; it cannot be used
//  in a single statement (e.g. as the body of an if statement without {})!
#define ASSIGN_OR_RETURN(lhs, ...)                                             \
  ASSIGN_OR_RETURN_IMPL(UTILS_CONCAT_NAME(_status_or_value, __COUNTER__), lhs, \
                        __VA_ARGS__);

#define DEFINE_OR_RETURN_IMPL(type, lhs, tmp, ...) \
  absl::StatusOr<type> tmp = (__VA_ARGS__);        \
  if (!(tmp).ok()) {                               \
    return (tmp).status();                         \
  }                                                \
  type &lhs = (tmp).value();  // NOLINT(bugprone-macro-parentheses)

// Executes an expression that returns an absl::StatusOr<T>, and defines a new
// variable with given type and name to the result if the error code is OK. If
// the Status is non-OK, returns the error.
#define DEFINE_OR_RETURN(type, lhs, ...)                                     \
  DEFINE_OR_RETURN_IMPL(type, lhs,                                           \
                        UTILS_CONCAT_NAME(__define_or_return_, __COUNTER__), \
                        __VA_ARGS__)
