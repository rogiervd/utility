load("@rules_cc//cc:defs.bzl", "cc_test")

def cc_test_utility(test_name, extra_deps = []):
    cc_test(
        name = test_name,
        size = "small",
        srcs = [test_name + ".cpp"],
        deps = [
            "//:utility",
            "@boost.test//:unit_test_main",
        ] + extra_deps,
    )
