
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "writer.h"
#include "close.h"

namespace sparkey {
  class LogWriterCloseWorker : public NanAsyncWorker {
    public:
      LogWriterCloseWorker(LogWriter *self, NanCallback *callback)
        : NanAsyncWorker(callback)
        , self(self) {}

      void
      Execute() {
        sparkey_returncode rc = sparkey_logwriter_close(&self->writer);
        self->writer = NULL;
        if (SPARKEY_SUCCESS != rc) {
          errmsg = strdup(sparkey_errstring(rc));
        }
      }

    private:
      LogWriter *self;
  };

  void
  LogWriterClose(LogWriter *self, NanCallback *callback) {
    LogWriterCloseWorker *worker = new LogWriterCloseWorker(self, callback);
    NanAsyncQueueWorker(worker);
  }

  void
  LogWriterCloseSync(LogWriter *self) {
    sparkey_returncode rc = sparkey_logwriter_close(&self->writer);
    self->writer = NULL;
    if (SPARKEY_SUCCESS != rc) {
      NanThrowError(sparkey_errstring(rc));
    }
  }
}
