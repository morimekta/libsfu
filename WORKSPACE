bind(
    name = "gtest",
    actual = "@googletest//:main"
)

git_repository(
    name = "googletest-bazel",
    remote = "git@github.com:chronos-tachyon/googletest-bazel.git",
    commit = "f3b46d5ce2637f8e72e49a05f6cd2fbf5bc62870",
)

new_http_archive(
    name = "googletest",
    url = "https://googletest.googlecode.com/files/gtest-1.7.0.zip",
    sha256 = "247ca18dd83f53deb1328be17e4b1be31514cedfc1e3424f672bf11fd7e0d60d",
    build_file = "gtest.BUILD",
    strip_prefix = "gtest-1.7.0/",
)
