
#ifndef NODE_SPARKEY_WRITER_WORKERS_H
#define NODE_SPARKEY_WRITER_WORKERS_H 1

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
};

#endif
