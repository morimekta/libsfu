cc_library(
    name = "main",
    srcs = glob(
        [
            "src/*.cc",
            "src/*.h",
        ],
        exclude = ["src/gtest-all.cc"]
    ),
    hdrs = glob([
        "include/**/*.h",
    ]),
    includes = [
        "include/"
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)
