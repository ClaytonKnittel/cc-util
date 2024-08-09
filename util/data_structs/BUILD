cc_library(
    name = "red_black_tree",
    srcs = ["red_black_tree.cc"],
    hdrs = ["red_black_tree.h"],
    deps = [
        "//util/internal:util",
    ],
)

cc_test(
    name = "red_black_tree_test",
    srcs = ["red_black_tree_test.cc"],
    deps = [
        ":red_black_tree",
        "//util:absl_util",
        "//util:gtest_util",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/strings:str_format",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
