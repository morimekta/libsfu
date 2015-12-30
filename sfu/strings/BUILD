cc_library(
    name = "cord",
    srcs = [ "cord.cc" ],
    hdrs = [ "cord.h" ],
    visibility = [ "//visibility:public" ],
)

cc_test(
    name = "cord_test",
    srcs = [ "cord_test.cc" ],
    deps = [
        ':cord',
        '//external:gtest',
    ],
    size = 'small',
)

cc_library(
    name = "format",
    srcs = [ "format.cc" ],
    hdrs = [ "format.h" ],
    visibility = [ "//visibility:public" ],
)

cc_test(
    name = "format_test",
    srcs = [ "format_test.cc" ],
    deps = [
        ':format',
        '//external:gtest',
    ],
    size = 'small',
)

cc_library(
    name = "strings",
    srcs = [ "strings.cc" ],
    hdrs = [ "strings.h" ],
    deps = [
        '//sfu:container-traits',
    ],
    visibility = [ "//visibility:public" ],
)

cc_test(
    name = "strings_test",
    srcs = [ "strings_test.cc" ],
    deps = [
        ':strings',
        '//external:gtest',
    ],
    size = 'small',
)

