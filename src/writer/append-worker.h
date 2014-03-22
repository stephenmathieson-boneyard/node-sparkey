
#ifndef NODE_SPARKEY_WRITER_APPEND_WORKER_H
#define NODE_SPARKEY_WRITER_APPEND_WORKER_H 1

#include <node.h>
#include <nan.h>
#include "writer.h"

namespace sparkey {
  class WriterAppendWorker : public NanAsyncWorker {
    public:
      WriterAppendWorker(
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
