{
  "targets": [
    {
      "target_name": "sparkey",
      "type": "static_library",
      "dependencies": ["snappy/snappy.gyp:snappy"],
      "include_dirs": ["sparkey/src/"],
      "direct_dependent_settings": {
        "include_dirs": ["sparkey/src"]
      },
      "conditions": [
        [ "OS=='linux'", {
          "cflags": [
            "-std=c99",
            "-fPIC",
          ],
        }],
      ],
      "sources": [
        "sparkey/src/buf.c",
        "sparkey/src/endiantools.c",
        "sparkey/src/hashalgorithms.c",
        "sparkey/src/hashheader.c",
        "sparkey/src/hashiter.c",
        "sparkey/src/hashreader.c",
        "sparkey/src/hashwriter.c",
        "sparkey/src/logheader.c",
        "sparkey/src/logreader.c",
        "sparkey/src/logwriter.c",
        "sparkey/src/MurmurHash3.c",
        "sparkey/src/returncodes.c",
        "sparkey/src/util.c",
        "sparkey/src/sparkey.h",
      ],
    },
  ]
}
