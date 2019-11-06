# -*-Python-*-

cc_library(name = "hash",
           srcs = ["hash.c", "hash_tofile.c", "hash_tostr.c", "memhash.c",
                   "u32hash.c"],
           hdrs = ["hash.h", "memhash.h", "u32hash.h"],
           deps = ["//:bkstyle", "//lines:lines", "//utils:utils"],
           visibility = ["//visibility:public"]
           )

cc_binary(name = "hash_test",
           srcs = ["hash_test.c"],
           deps = ["//:bksupport"])
