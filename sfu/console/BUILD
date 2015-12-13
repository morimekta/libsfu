cc_library(
    name = "argument_parser",
    srcs = [ "argument_parser.cc" ],
    hdrs = [ "argument_parser.h" ],
    visibility = [ "//visibility:public" ],
    deps = [
        '//sfu/strings:cord',
        '//sfu:nullstream',
        '//sfu:numbers',
    ],
)

cc_test(
    name = "argument_parser_test",
    srcs = [ "argument_parser_test.cc" ],
    deps = [
        ':argument_parser',
        '//sfu:numbers',
        '//third_party:gtest',
    ],
    size = 'small',
)

cc_library(
    name = "char",
    srcs = [ "char.cc" ],
    hdrs = [ "char.h" ],
    visibility = [ "//visibility:public" ],
    deps = [
        '//sfu/strings:format',
        '//sfu:utf8',
        '//sfu:encoding',
    ],
)

cc_test(
    name = "char_test",
    srcs = [ "char_test.cc" ],
    deps = [
        ':char',
        '//sfu:encoding',
        '//third_party:gtest',
    ],
    size = 'small',
)

cc_library(
    name = "input",
    srcs = [ "input.cc" ],
    hdrs = [ "input.h" ],
    visibility = [ "//visibility:public" ],
    deps = [
        ':char',
        '//sfu:utf8',
    ],
)

cc_library(
    name = "interactive",
    srcs = [ "interactive.cc" ],
    hdrs = [ "interactive.h" ],
    visibility = [ "//visibility:public" ],
    deps = [
        ':char',
        ':input',
        '//sfu:utf8',
    ],
)

cc_binary(
    name = "interactive_test_main",
    srcs = [ "interactive_test_main.cc" ],
    visibility = [ "//visibility:public" ],
    deps = [
        ':interactive',
        '//sfu:encoding',
    ],
)

cc_library(
    name = "output",
    srcs = [ "output.cc" ],
    hdrs = [ "output.h" ],
    visibility = [ "//visibility:public" ],
)

