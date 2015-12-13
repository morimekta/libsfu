cc_library(
    name = "container-traits",
    srcs = [ "container_traits.cc" ],
    hdrs = [ "container_traits.h" ],
    visibility = [ "//visibility:public" ],
)

cc_library(
    name = "encoding",
    srcs = [ "encoding.cc" ],
    hdrs = [ "encoding.h" ],
    deps = [
        "//sfu/strings:cord",
        "//sfu:utf8",
    ],
    visibility = [ "//visibility:public" ],
)

cc_test(
    name = "encoding_test",
    srcs = [ "encoding_test.cc" ],
    deps = [
        ':encoding',
        '//third_party:gtest',
    ],
    size = 'small',
)

cc_library(
    name = "nullstream",
    srcs = [ "nullstream.cc" ],
    hdrs = [ "nullstream.h" ],
    visibility = [ "//visibility:public" ],
)

cc_library(
    name = "numbers",
    srcs = [ "numbers.cc" ],
    hdrs = [ "numbers.h" ],
    deps = [
        "//sfu/strings:cord",
    ],
    visibility = [ "//visibility:public" ],
)

cc_test(
    name = "numbers_test",
    srcs = [ "numbers_test.cc" ],
    deps = [
        ':numbers',
        '//third_party:gtest',
    ],
    size = 'small',
)

cc_library(
    name = "path",
    srcs = [ "path.cc" ],
    hdrs = [ "path.h" ],
    deps = [
        "//sfu/strings:strings",
    ],
    visibility = [ "//visibility:public" ],
)

cc_library(
    name = "popen",
    srcs = [ "popen.cc" ],
    hdrs = [ "popen.h" ],
    visibility = [ "//visibility:public" ],
)

cc_test(
    name = "popen_test",
    srcs = [ "popen_test.cc" ],
    deps = [
        ':popen',
        '//third_party:gtest',
    ],
    size = 'small',
)

cc_library(
    name = "tmpfile",
    srcs = [ "tmpfile.cc" ],
    deps = [
        "//sfu/strings:cord",
        "//sfu:encoding",
    ],
    hdrs = [ "tmpfile.h" ],
    visibility = [ "//visibility:public" ],
)

cc_library(
    name = "utf8",
    srcs = [ "utf8.cc" ],
    hdrs = [ "utf8.h" ],
    visibility = [ "//visibility:public" ],
)
