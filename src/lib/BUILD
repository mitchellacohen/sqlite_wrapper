load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_cc//cc:defs.bzl", "cc_test")

cc_library(
    name = "sqlite_wrapper",
    srcs = ["sqlite_wrapper.cc"],
    hdrs = ["sqlite_wrapper.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":sqlite3",
        "@com_google_absl//absl/status:status",
        "@com_google_absl//absl/strings:strings",
    ],
)

cc_library(
    name = "sqlite3",
    srcs = ["sqlite3.c"],
    hdrs = ["sqlite3.h"],
    visibility = ["//visibility:public"],
    deps = [],
)

cc_binary(
    name = "sqlite_sample_main",
    srcs = ["sqlite_sample_main.cc"],
    deps = [
        ":sqlite_wrapper",
        "@com_google_absl//absl/status:status",
    ],
    linkopts = ["-std=c++17"],
    copts = ["-std=c++17"],
)

cc_test(
    name = "sqlite_wrapper_test",
    srcs = glob(["sqlite_wrapper_test.cc"]),
    deps = [
        ":sqlite_wrapper",
        "@com_google_absl//absl/types:optional",
        "@googletest//:gtest_main",
    ],
)
