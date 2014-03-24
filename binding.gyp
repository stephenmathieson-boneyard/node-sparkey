{
  "targets": [
    {
      "target_name": "sparkey",
      "dependencies": [
        "<(module_root_dir)/deps/sparkey.gyp:sparkey",
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
      ],
      "sources": [
        "src/sparkey.cc",
        "src/hash.cc",

        "src/writer/writer.cc",
        "src/writer/workers.cc",

        "src/reader/reader.cc",
        "src/reader/iterator.cc",
        "src/reader/workers.cc",
      ],
    }
  ]
}
