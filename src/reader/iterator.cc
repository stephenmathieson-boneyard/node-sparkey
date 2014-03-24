

#include <node.h>
#include <nan.h>
#include "reader.h"
#include "iterator.h"
#include "workers.h"

namespace sparkey {
  v8::Persistent<v8::FunctionTemplate> LogReaderIterator::constructor;

  LogReaderIterator::LogReaderIterator() {}

  LogReaderIterator::~LogReaderIterator() {}

  void
  LogReaderIterator::Init() {
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(
      LogReaderIterator::New
    );
    NanAssignPersistent(v8::FunctionTemplate, constructor, tpl);
    tpl->SetClassName(NanSymbol("LogReaderIterator"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "end", End);
    NODE_SET_PROTOTYPE_METHOD(tpl, "next", Next);
    NODE_SET_PROTOTYPE_METHOD(tpl, "skip", Skip);
    NODE_SET_PROTOTYPE_METHOD(tpl, "isActive", IsActive);
  }

  NAN_METHOD(LogReaderIterator::New) {
    NanScope();
    LogReader *reader = ObjectWrap::Unwrap<LogReader>(
      args[0]->ToObject()
    );
    LogReaderIterator *self = new LogReaderIterator;
    sparkey_returncode rc;
    self->reader = reader->reader;
    rc = sparkey_logiter_create(&self->iterator, self->reader);
    if (SPARKEY_SUCCESS != rc) {
      NanThrowError(sparkey_errstring(rc));
    }
    self->Wrap(args.This());
    NanReturnValue(args.This());
  }

  NAN_METHOD(LogReaderIterator::End) {
    NanScope();
    LogReaderIterator *self = ObjectWrap::Unwrap<LogReaderIterator>(
      args.This()
    );
    sparkey_logiter_close(&self->iterator);
    NanReturnUndefined();
  }

  NAN_METHOD(LogReaderIterator::Next) {
    NanScope();
    LogReaderIterator *self = ObjectWrap::Unwrap<LogReaderIterator>(
      args.This()
    );
    v8::Local<v8::Function> fn = args[0].As<v8::Function>();
    LogReaderIteratorNextWorker *worker = new LogReaderIteratorNextWorker(
        self
      , new NanCallback(fn)
    );
    NanAsyncQueueWorker(worker);
    NanReturnUndefined();
  }

  NAN_METHOD(LogReaderIterator::Skip) {
    NanScope();
    LogReaderIterator *self = ObjectWrap::Unwrap<LogReaderIterator>(
      args.This()
    );
    int number = args[0]->NumberValue();
    v8::Local<v8::Function> fn = args[1].As<v8::Function>();
    LogReaderIteratorSkipWorker *worker = new LogReaderIteratorSkipWorker(
        self
      , number
      , new NanCallback(fn)
    );
    NanAsyncQueueWorker(worker);
    NanReturnUndefined();
  }

  NAN_METHOD(LogReaderIterator::IsActive) {
    NanScope();
    LogReaderIterator *self = ObjectWrap::Unwrap<LogReaderIterator>(
      args.This()
    );
    bool is_active = SPARKEY_ITER_ACTIVE == sparkey_logiter_state(
      self->iterator
    );
    NanReturnValue(v8::Boolean::New(is_active));
  }

  v8::Local<v8::Object>
  LogReaderIterator::NewInstance(v8::Local<v8::Object> reader) {
    NanScope();
    v8::Local<v8::Object> instance;
    v8::Local<v8::FunctionTemplate> c = NanPersistentToLocal(
      constructor
    );
    v8::Handle<v8::Value> argv[1] = { reader };
    instance = c->GetFunction()->NewInstance(1, argv);
    return instance;
  }
}
