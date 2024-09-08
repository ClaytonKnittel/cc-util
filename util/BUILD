cc_library(
    name = "absl_util",
    hdrs = ["absl_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":macro_util",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ],
)

cc_library(
    name = "bit_set",
    hdrs = ["bit_set.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@abseil-cpp//absl/numeric:bits",
    ],
)

cc_test(
    name = "bit_set_test",
    srcs = ["bit_set_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":bit_set",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "csi",
    hdrs = ["csi.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gtest_util",
    hdrs = ["gtest_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/strings",
        "@googletest//:gtest",
    ],
)

cc_test(
    name = "gtest_util_test",
    srcs = ["gtest_util_test.cc"],
    deps = [
        ":absl_util",
        ":gtest_util",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "macro_util",
    hdrs = ["macro_util.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "print_colors",
    hdrs = ["print_colors.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "std_util",
    hdrs = ["std_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":macro_util",
    ],
)
