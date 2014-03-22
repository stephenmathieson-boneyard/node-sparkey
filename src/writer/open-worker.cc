
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "writer.h"
#include "open-worker.h"

namespace sparkey {
  WriterOpenWorker::WriterOpenWorker(
        LogWriter *self
      , int block_size
      , sparkey_compression_type compression_type
      , NanCallback *callback
    ) : NanAsyncWorker(callback)
      , self(self)
      , block_size(block_size)
      , compression_type(compression_type) {}

  void
  WriterOpenWorker::Execute() {
    sparkey_returncode rc;
    rc = sparkey_logwriter_create(
        &self->writer
      , self->path
      , compression_type
      , block_size
    );
    if (SPARKEY_SUCCESS != rc) {
      errmsg = strdup(sparkey_errstring(rc));
    }
  }
}
