
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "writer.h"
#include "close-worker.h"

namespace sparkey {
  WriterCloseWorker::WriterCloseWorker(
        LogWriter *self
      , NanCallback *callback
    ) : NanAsyncWorker(callback)
      , self(self) {}

  void
  WriterCloseWorker::Execute() {
    sparkey_returncode rc = sparkey_logwriter_close(&self->writer);
    if (SPARKEY_SUCCESS != rc) {
      errmsg = strdup(sparkey_errstring(rc));
    }
  }
}
