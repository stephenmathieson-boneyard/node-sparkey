
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "writer.h"
#include "append-worker.h"

namespace sparkey {
  WriterAppendWorker::WriterAppendWorker(
        LogWriter *self
      , NanCallback *callback
    ) : NanAsyncWorker(callback)
      , self(self) {}

  void
  WriterAppendWorker::Execute() {
    sparkey_returncode rc;
    rc = sparkey_logwriter_append(
        &self->writer
      , self->path
    );
    if (SPARKEY_SUCCESS != rc) {
      errmsg = strdup(sparkey_errstring(rc));
    }
  }
}
