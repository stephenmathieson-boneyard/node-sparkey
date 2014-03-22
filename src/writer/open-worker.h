
#ifndef NODE_SPARKEY_WRITER_OPEN_WORKER_H
#define NODE_SPARKEY_WRITER_OPEN_WORKER_H 1

#include <node.h>
#include <nan.h>
#include "writer.h"

namespace sparkey {
  class WriterOpenWorker : public NanAsyncWorker {
    public:
      WriterOpenWorker(
          LogWriter *
        , int
        , sparkey_compression_type
        , NanCallback *
      );

      void
      Execute();

    private:
      LogWriter *self;
      int block_size;
      sparkey_compression_type compression_type;
  };
}

#endif
