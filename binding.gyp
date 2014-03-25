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
        "src/log-writer/writer.cc",
        "src/log-writer/workers.cc",
        "src/log-reader/reader.cc",
        "src/log-reader/iterator.cc",
        "src/log-reader/workers.cc",
        "src/hash-reader/reader.cc",
        "src/hash-reader/iterator.cc",
      ],
    }
  ]
}
