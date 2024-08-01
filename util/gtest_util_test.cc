#include "util/gtest_util.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace util {

using ::testing::Not;

TEST(GTestUtilTest, TestReturnIfError) {
  auto test = []() -> absl::Status {
    RETURN_IF_ERROR(absl::InternalError(""));
    return absl::OkStatus();
  };

  EXPECT_THAT(test(), Not(IsOk()));
}

template <typename T, typename U>
absl::StatusOr<T> TestFn() {
  return absl::InternalError("");
}

TEST(GTestUtilTest, TestAssignOrReturn) {
  auto test = []() -> absl::StatusOr<uint64_t> {
    uint64_t val;
    ASSIGN_OR_RETURN(val, TestFn<uint64_t, uint32_t>());
    return val + 1;
  };

  EXPECT_THAT(test(), Not(IsOk()));
}

template <typename T, typename U>
absl::StatusOr<T> TestFn2(T val) {
  return 2 * val;
}

TEST(GTestUtilTest, TestDefineOrReturn) {
  auto test = []() -> absl::StatusOr<uint64_t> {
    DEFINE_OR_RETURN(uint64_t, val, TestFn2<uint64_t, uint32_t>(2));
    return val + 1;
  };

  EXPECT_THAT(test(), IsOkAndHolds(5));
}

}  // namespace util
