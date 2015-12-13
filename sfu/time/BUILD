cc_library(
    name = "datetime",
    srcs = [ "datetime.cc" ],
    hdrs = [ "datetime.h" ],
    visibility = [ "//visibility:public" ],
)

cc_library(
    name = "format",
    srcs = [ "format.cc" ],
    hdrs = [ "format.h" ],
    visibility = [ "//visibility:public" ],
)

cc_library(
    name = "time",
    srcs = [ "time.cc" ],
    hdrs = [ "time.h" ],
    visibility = [ "//visibility:public" ],
)

cc_library(
    name = "zoneinfo",
    srcs = [ "zoneinfo.cc" ],
    hdrs = [ "zoneinfo.h" ],
    deps = [
        ":time",
        "//sfu/strings:strings",
        "//sfu:numbers",
        "//sfu:popen",
    ],
    visibility = [ "//visibility:public" ],
)

