
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "reader.h"
#include "iterator.h"

namespace sparkey {

v8::Persistent<v8::FunctionTemplate> LogReader::constructor;

/**
 * Close worker.
 */

class LogReaderCloseWorker : public NanAsyncWorker {
  public:
    LogReaderCloseWorker(
        LogReader *self
      , NanCallback *callback
    ) : NanAsyncWorker(callback), self(self) {}

    void
    Execute() {
      self->CloseReader();
    };

  private:
    LogReader *self;
};

/**
 * Open worker.
 */

class LogReaderOpenWorker : public NanAsyncWorker {
  public:
    LogReaderOpenWorker(
        LogReader *self
      , NanCallback *callback
    ) : NanAsyncWorker(callback), self(self) {}

    void
    Execute() {
      sparkey_returncode rc = self->OpenReader();
      if (SPARKEY_SUCCESS != rc) {
        // most obnixous part of NAN: they free `errmsg`, so
        // you've got to use a pointer you own :(
        errmsg = strdup(sparkey_errstring(rc));
      }
    }

  private:
    LogReader *self;
};

LogReader::LogReader() {
  reader = NULL;
  iterator = NULL;
  is_open = false;
}

LogReader::~LogReader() {
  delete path;
}

sparkey_returncode
LogReader::OpenReader() {
  // noop if we're already open
  if (is_open) return SPARKEY_SUCCESS;
  sparkey_returncode rc = sparkey_logreader_open(&reader, path);
  if (SPARKEY_SUCCESS == rc) is_open = true;
  return rc;
}

void
LogReader::CloseReader() {
  // noop if we're already closed
  if (!is_open) return;
  sparkey_logreader_close(&reader);
  is_open = false;
}

/*
 * Methods exposed to v8.
 */

void
LogReader::Init(v8::Handle<v8::Object> exports) {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(
    LogReader::New
  );
  NanAssignPersistent(v8::FunctionTemplate, constructor, tpl);

  tpl->SetClassName(NanSymbol("LogReader"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
  NODE_SET_PROTOTYPE_METHOD(tpl, "closeSync", CloseSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "openSync", OpenSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getBlockSize", GetBlockSize);
  NODE_SET_PROTOTYPE_METHOD(tpl, "iterator", NewIterator);

  exports->Set(NanSymbol("LogReader"), tpl->GetFunction());
}

NAN_METHOD(LogReader::New) {
  NanScope();
  LogReader *self = new LogReader;
  size_t pathsize;
  self->path = NanCString(args[0], &pathsize);
  self->block_size = -1;
  self->Wrap(args.This());
  NanReturnValue(args.This());
}

NAN_METHOD(LogReader::Close) {
  NanScope();
  LogReader *self = ObjectWrap::Unwrap<LogReader>(args.This());
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  LogReaderCloseWorker *worker = new LogReaderCloseWorker(
      self
    , new NanCallback(fn)
  );
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(LogReader::CloseSync) {
  NanScope();
  LogReader *self = ObjectWrap::Unwrap<LogReader>(args.This());
  self->CloseReader();
  NanReturnUndefined();
}

NAN_METHOD(LogReader::Open) {
  NanScope();
  LogReader *self = ObjectWrap::Unwrap<LogReader>(args.This());
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  LogReaderOpenWorker *worker = new LogReaderOpenWorker(
      self
    , new NanCallback(fn)
  );
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(LogReader::OpenSync) {
  NanScope();
  LogReader *self = ObjectWrap::Unwrap<LogReader>(args.This());
  sparkey_returncode rc = self->OpenReader();
  if (SPARKEY_SUCCESS != rc) {
    NanThrowError(sparkey_errstring(rc));
  }
  NanReturnUndefined();
}

// TODO this should probably just be a getter function (reader.blockSize)
NAN_METHOD(LogReader::GetBlockSize) {
  NanScope();
  LogReader *self = ObjectWrap::Unwrap<LogReader>(args.This());
  // only lookup once, as it's constant
  if (-1 == self->block_size) {
    self->block_size = sparkey_logreader_get_compression_blocksize(
      self->reader
    );
  }
  NanReturnValue(v8::Number::New(self->block_size));
}

NAN_METHOD(LogReader::NewIterator) {
  NanScope();
  NanReturnValue(LogReaderIterator::NewInstance(args.This()));
}

} // namespace sparkey
