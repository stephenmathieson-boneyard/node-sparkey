
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "writer.h"
#include "open.h"

namespace sparkey {
  class LogWriterOpenWorker : public NanAsyncWorker {
    public:
      LogWriterOpenWorker(
            LogWriter *self
          , sparkey_compression_type compression_type
          , int block_size
          , NanCallback *callback
        ) : NanAsyncWorker(callback)
          , self(self)
          , block_size(block_size)
          , compression_type(compression_type) {}

      void
      Execute() {
        sparkey_returncode rc = sparkey_logwriter_create(&self->writer
          , self->path
          , compression_type
          , block_size);
        if (SPARKEY_SUCCESS != rc) {
          errmsg = strdup(sparkey_errstring(rc));
        }
      }

    private:
      LogWriter *self;
      int block_size;
      sparkey_compression_type compression_type;
  };

  void
  LogWriterOpen(LogWriter *self
      , sparkey_compression_type compression_type
      , int block_size
      , NanCallback *callback
    ) {
    LogWriterOpenWorker *worker = new LogWriterOpenWorker(self
      , compression_type
      , block_size
      , callback);
    NanAsyncQueueWorker(worker);
  }

  void
  LogWriterOpenSync(LogWriter *self
      , sparkey_compression_type compression_type
      , int block_size
    ) {
    sparkey_returncode rc = sparkey_logwriter_create(&self->writer
      , self->path
      , compression_type
      , block_size);
    if (SPARKEY_SUCCESS != rc) {
      NanThrowError(sparkey_errstring(rc));
    }
  }
}
