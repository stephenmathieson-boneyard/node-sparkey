

#include <node.h>
#include <nan.h>
#include "reader.h"
#include "iterator.h"
#include "iterator-next.h"

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
    NODE_SET_PROTOTYPE_METHOD(tpl, "end", LogReaderIterator::End);
    NODE_SET_PROTOTYPE_METHOD(tpl, "next", LogReaderIterator::Next);
  }

  NAN_METHOD(LogReaderIterator::New) {
    NanScope();
    LogReader *reader = node::ObjectWrap::Unwrap<LogReader>(
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
    LogReaderIterator *self = node::ObjectWrap::Unwrap<LogReaderIterator>(
      args.This()
    );
    sparkey_logiter_close(&self->iterator);
    NanReturnUndefined();
  }

  NAN_METHOD(LogReaderIterator::Next) {
    NanScope();
    LogReaderIterator *self = node::ObjectWrap::Unwrap<LogReaderIterator>(
      args.This()
    );
    v8::Local<v8::Function> fn = args[0].As<v8::Function>();
    sparkey::LogReaderIteratorNext(self, new NanCallback(fn));
    NanReturnUndefined();
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
