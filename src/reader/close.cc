
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "reader.h"
#include "close.h"

namespace sparkey {

  /**
   * Pointless asynchronous close worker.
   */

  class LogReaderCloseWorker : public NanAsyncWorker {
    public:
      LogReaderCloseWorker(LogReader *self, NanCallback *callback)
        : NanAsyncWorker(callback)
        , self(self) {}

      void
      Execute() {
        sparkey_logreader_close(&self->reader);
      }

    private:
      LogReader *self;
  };

  void
  LogReaderClose(LogReader *self, NanCallback *callback) {
    LogReaderCloseWorker *worker = new LogReaderCloseWorker(self, callback);
    NanAsyncQueueWorker(worker);
  }

  void
  LogReaderCloseSync(LogReader *self) {
    sparkey_logreader_close(&self->reader);
  }
}
