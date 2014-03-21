
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "reader.h"
#include "close.h"

namespace sparkey {
  class LogReaderOpenWorker : public NanAsyncWorker {
    public:
      LogReaderOpenWorker(LogReader *self, NanCallback *callback)
        : NanAsyncWorker(callback)
        , self(self) {}

      void
      Execute() {
        sparkey_returncode rc;
        rc = sparkey_logreader_open(&self->reader, self->path);
        if (SPARKEY_SUCCESS != rc) {
          // most obnixous part of NAN: they free `errmsg`, so
          // you've got to use a pointer you own :(
          errmsg = strdup(sparkey_errstring(rc));
        }
      }

    private:
      LogReader *self;
  };

  void
  LogReaderOpen(LogReader *self, NanCallback *callback) {
    LogReaderOpenWorker *worker = new LogReaderOpenWorker(self, callback);
    NanAsyncQueueWorker(worker);
  }

  void
  LogReaderOpenSync(LogReader *self) {
    sparkey_returncode rc;
    rc = sparkey_logreader_open(&self->reader, self->path);
    if (SPARKEY_SUCCESS != rc) {
      NanThrowError(sparkey_errstring(rc));
    }
  }
}
