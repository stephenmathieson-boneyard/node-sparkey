
#ifndef NODE_SPARKEY_WRITER_CLOSE_WORKER_H
#define NODE_SPARKEY_WRITER_CLOSE_WORKER_H 1

#include <node.h>
#include <nan.h>
#include "writer.h"

namespace sparkey {
  class WriterCloseWorker : public NanAsyncWorker {
    public:
      WriterCloseWorker(
          LogWriter *
        , NanCallback *
      );

      void
      Execute();

    private:
      LogWriter *self;
  };
}

#endif
