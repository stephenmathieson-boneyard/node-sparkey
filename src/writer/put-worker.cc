
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "writer.h"
#include "put-worker.h"

namespace sparkey {
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
