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
        "src/writer/writer.cc",
        "src/writer/put.cc",
        "src/writer/close.cc",
        "src/writer/open.cc",
        "src/reader/reader.cc",
        "src/reader/close.cc",
        "src/reader/open.cc",
        "src/reader/iterator.cc",
        "src/reader/iterator-next.cc",
      ],
    }
  ]
}
