
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "writer.h"
#include "workers.h"

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

  WriterPutWorker::WriterPutWorker(
      LogWriter *self
    , char *key
    , size_t keysize
    , char *value
    , size_t valuesize
    , NanCallback *callback
  ) : NanAsyncWorker(callback)
    , self(self)
    , keysize(keysize)
    , valuesize(valuesize)
    , key(key)
    , value(value) {}

  void
  WriterPutWorker::Execute() {
    sparkey_returncode rc = sparkey_logwriter_put(
        self->writer
      , keysize
      , (uint8_t *) key
      , valuesize
      , (uint8_t *) value
    );

    delete key;
    delete value;

    if (SPARKEY_SUCCESS != rc) {
      errmsg = strdup(sparkey_errstring(rc));
    }
  }
}
