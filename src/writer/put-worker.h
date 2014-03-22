
#ifndef NODE_SPARKEY_WRITER_PUT_WORKER_H
#define NODE_SPARKEY_WRITER_PUT_WORKER_H 1

#include <node.h>
#include <nan.h>
#include "writer.h"

namespace sparkey {
  class WriterPutWorker : public NanAsyncWorker {
    public:
      WriterPutWorker(
          LogWriter *
        , char *key
        , size_t keysize
        , char *value
        , size_t valuesize
        , NanCallback *
      );

      void
      Execute();

    private:
      LogWriter *self;
      size_t keysize;
      size_t valuesize;
      char *key;
      char *value;
  };
}

#endif
