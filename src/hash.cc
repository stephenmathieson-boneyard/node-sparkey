
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "hash.h"

namespace sparkey {
  class HashWorker : public NanAsyncWorker {
    public:
      HashWorker(
          char *log_file
        , char *hash_file
        , int hash_size
        , NanCallback *callback
      ) : NanAsyncWorker(callback)
        , log_file(log_file)
        , hash_file(hash_file)
        , hash_size(hash_size) {}

      void
      Execute() {
        sparkey_returncode rc;
        rc = sparkey_hash_write(hash_file, log_file, hash_size);
        if (SPARKEY_SUCCESS != rc) {
          errmsg = strdup(sparkey_errstring(rc));
        }
      }

    private:
      char *log_file;
      char *hash_file;
      int hash_size;
  };

  NAN_METHOD(Hash) {
    NanScope();
    size_t logsize;
    size_t hashsize;
    char *log_file = NanCString(args[0], &logsize);
    char *hash_file = NanCString(args[1], &hashsize);
    // TODO option
    int hash_size = 0;
    v8::Local<v8::Function> fn = args[2].As<v8::Function>();
    HashWorker *worker = new HashWorker(
        log_file
      , hash_file
      , hash_size
      , new NanCallback(fn)
    );
    NanAsyncQueueWorker(worker);
    NanReturnUndefined();
  }

  NAN_METHOD(HashSync) {
    NanScope();
    size_t logsize;
    size_t hashsize;
    char *log_file = NanCString(args[0], &logsize);
    char *hash_file = NanCString(args[1], &hashsize);
    // TODO option
    int hash_size = 0;
    sparkey_returncode rc = sparkey_hash_write(hash_file, log_file, hash_size);
    if (SPARKEY_SUCCESS != rc) {
      NanThrowError(sparkey_errstring(rc));
    }
    NanReturnUndefined();
  }

  void
  InitHash(v8::Handle<v8::Object> exports) {
    exports->Set(NanSymbol("hash"), v8::FunctionTemplate::New(
      Hash)->GetFunction()
    );
    exports->Set(NanSymbol("hashSync"), v8::FunctionTemplate::New(
      HashSync)->GetFunction()
    );
  }
}
