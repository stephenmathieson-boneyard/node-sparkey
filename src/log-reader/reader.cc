
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "reader.h"
#include "workers.h"
#include "iterator.h"

namespace sparkey {

  v8::Persistent<v8::FunctionTemplate> LogReader::constructor;

  LogReader::LogReader() {
    reader = NULL;
    iterator = NULL;
  }
  LogReader::~LogReader() {
    delete path;
  }

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

    NODE_SET_PROTOTYPE_METHOD(tpl, "iterator", NewIterator);

    exports->Set(NanSymbol("LogReader"), constructor->GetFunction());
  }

  NAN_METHOD(LogReader::New) {
    NanScope();
    LogReader *self = new LogReader;
    size_t pathsize;
    self->path = NanCString(args[0], &pathsize);
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
    sparkey_logreader_close(&self->reader);
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
    sparkey_returncode rc = sparkey_logreader_open(&self->reader, self->path);
    if (SPARKEY_SUCCESS != rc) {
      NanThrowError(sparkey_errstring(rc));
    }
    NanReturnUndefined();
  }

  NAN_METHOD(LogReader::NewIterator) {
    NanScope();
    NanReturnValue(LogReaderIterator::NewInstance(args.This()));
  }
}
