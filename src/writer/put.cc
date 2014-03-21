
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "writer.h"
#include "put.h"

namespace sparkey {
  class LogWriterPutWorker : public NanAsyncWorker {
    public:
      LogWriterPutWorker(
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
      Execute() {
        sparkey_returncode rc = sparkey_logwriter_put(self->writer
          , keysize
          , (uint8_t *) key
          , valuesize
          , (uint8_t *) value);

        delete key;
        delete value;

        if (SPARKEY_SUCCESS != rc) {
          errmsg = strdup(sparkey_errstring(rc));
        }
      }

    private:
      LogWriter *self;
      size_t keysize;
      size_t valuesize;
      char *key;
      char *value;
  };

  void
  LogWriterPut(LogWriter *writer
      , char *key
      , size_t keysize
      , char *value
      , size_t valuesize
      , NanCallback *callback
    ) {
    LogWriterPutWorker *worker = new LogWriterPutWorker(writer
      , key
      , keysize
      , value
      , valuesize
      , callback);
    NanAsyncQueueWorker(worker);
  }

  void
  LogWriterPutSync(LogWriter *writer
      , char *key
      , size_t keysize
      , char *value
      , size_t valuesize
    ) {
    sparkey_returncode rc = sparkey_logwriter_put(writer->writer
      , keysize
      , (uint8_t *) key
      , valuesize
      , (uint8_t *) value);

    delete key;
    delete value;

    if (SPARKEY_SUCCESS != rc) {
      NanThrowError(sparkey_errstring(rc));
    }
  }
}
