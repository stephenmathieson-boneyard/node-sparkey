
#ifndef NODE_SPARKEY_LOG_READER_WORKERS_H
#define NODE_SPARKEY_LOG_READER_WORKERS_H 1

#include <node.h>
#include <nan.h>
#include "reader.h"
#include "iterator.h"

namespace sparkey {
  class LogReaderCloseWorker : public NanAsyncWorker {
    public:
      LogReaderCloseWorker(
          LogReader *
        , NanCallback *
      );

      void
      Execute();

    private:
      LogReader *self;
  };

  class LogReaderOpenWorker : public NanAsyncWorker {
    public:
      LogReaderOpenWorker(
          LogReader *
        , NanCallback *
      );

      void
      Execute();

    private:
      LogReader *self;
  };

  class LogReaderIteratorNextWorker : public NanAsyncWorker {
    public:
      LogReaderIteratorNextWorker(
          LogReaderIterator *
        , NanCallback *
      );

      ~LogReaderIteratorNextWorker();

      void
      Execute();

      void
      HandleOKCallback();

    private:
      LogReaderIterator *self;
      char *key;
      char *value;
      IteratorEntryType type;
  };

  class LogReaderIteratorSkipWorker : public NanAsyncWorker {
    public:
      LogReaderIteratorSkipWorker(
          LogReaderIterator *
        , int number
        , NanCallback *
      );

      void
      Execute();

    private:
      LogReaderIterator *self;
      int number;
  };
}

#endif
