cc_library(
    name = "gtest_util",
    hdrs = ["gtest_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/strings",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "gtest_util_test",
    srcs = ["gtest_util_test.cc"],
    deps = [
        ":gtest_util",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
