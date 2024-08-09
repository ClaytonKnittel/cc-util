#pragma once

// Internal helper for concatenating macro values.
#define UTILS_CONCAT_NAME_INNER(x, y) x##y
#define UTILS_CONCAT_NAME(x, y)       UTILS_CONCAT_NAME_INNER(x, y)
