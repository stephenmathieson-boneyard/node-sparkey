
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "reader.h"
#include "iterator.h"

namespace sparkey {

class HashReaderOpenWorker : public NanAsyncWorker {
  public:
    HashReaderOpenWorker(
        HashReader *self
      , NanCallback *callback
    ) : NanAsyncWorker(callback), self(self) {}

    void SetError(sparkey_returncode rc) {
      errmsg = strdup(sparkey_errstring(rc));
    }

    void
    Execute() {
      sparkey_returncode rc;

      rc = sparkey_hash_open(
          &self->hash_reader
        , self->hashfile
        , self->logfile
      );
      if (SPARKEY_SUCCESS != rc) {
        return SetError(rc);
      }
    }

  private:
    HashReader *self;
};

class HashReaderCloseWorker : public NanAsyncWorker {
  public:
    HashReaderCloseWorker(
        HashReader *self
      , NanCallback *callback
    ) : NanAsyncWorker(callback), self(self) {}

    void
    Execute() {
      sparkey_hash_close(&self->hash_reader);
    }

  private:
    HashReader *self;
};

v8::Persistent<v8::FunctionTemplate> HashReader::constructor;

HashReader::HashReader() {
  log_reader = NULL;
  hash_reader = NULL;
}

HashReader::~HashReader() {
  delete logfile;
  delete hashfile;
}

void
HashReader::Init(v8::Handle<v8::Object> exports) {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(
    HashReader::New
  );
  NanAssignPersistent(v8::FunctionTemplate, constructor, tpl);

  tpl->SetClassName(NanSymbol("HashReader"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "count", Count);
  NODE_SET_PROTOTYPE_METHOD(tpl, "iterator", NewIterator);

  exports->Set(NanSymbol("HashReader"), tpl->GetFunction());
}

NAN_METHOD(HashReader::New) {
  NanScope();
  HashReader *self = new HashReader;
  size_t hashsize;
  size_t logsize;
  self->hashfile = NanCString(args[0], &hashsize);
  self->logfile = NanCString(args[1], &logsize);
  self->Wrap(args.This());
  NanReturnValue(args.This());
}

NAN_METHOD(HashReader::Open) {
  NanScope();
  HashReader *self = ObjectWrap::Unwrap<HashReader>(args.This());
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  HashReaderOpenWorker *worker = new HashReaderOpenWorker(
      self
    , new NanCallback(fn)
  );
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(HashReader::Close) {
  NanScope();
  HashReader *self = ObjectWrap::Unwrap<HashReader>(args.This());
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  HashReaderCloseWorker *worker = new HashReaderCloseWorker(
      self
    , new NanCallback(fn)
  );
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(HashReader::Count) {
  NanScope();
  HashReader *self = ObjectWrap::Unwrap<HashReader>(args.This());
  uint64_t count = sparkey_hash_numentries(self->hash_reader);
  NanReturnValue(v8::Number::New(count));
}

NAN_METHOD(HashReader::NewIterator) {
  NanScope();
  NanReturnValue(
    HashIterator::NewInstance(
      args.This()
    )
  );
}

} // namespace sparkey
