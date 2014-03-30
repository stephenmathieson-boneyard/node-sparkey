
#include <stdlib.h>
#include <node.h>
#include <nan.h>
#include "reader.h"
#include "iterator.h"

namespace sparkey {

v8::Persistent<v8::FunctionTemplate> LogReaderIterator::constructor;

/**
 * Skip worker.
 */

class LogReaderIteratorSkipWorker : public NanAsyncWorker {
  public:
    LogReaderIteratorSkipWorker(
        LogReaderIterator *self
      , int number
      , NanCallback *callback
    ) : NanAsyncWorker(callback), self(self), number(number) {}

    void
    Execute() {
      sparkey_returncode rc = sparkey_logiter_skip(
          self->iterator
        , self->reader
        , number
      );
      if (SPARKEY_SUCCESS != rc) {
        errmsg = strdup(sparkey_errstring(rc));
        return;
      }
    }

  private:
    LogReaderIterator *self;
    int number;
};

/**
 * Next worker.
 */

class LogReaderIteratorNextWorker : public NanAsyncWorker {
  public:
    LogReaderIteratorNextWorker(
        LogReaderIterator *self
      , NanCallback *callback
    ) : NanAsyncWorker(callback), self(self) {
      key = NULL;
      value = NULL;
    }


    ~LogReaderIteratorNextWorker() {
      delete key;
      delete value;
    }

    void
    Execute() {
      sparkey_returncode rc;
      uint64_t wanted_keylen;
      uint64_t wanted_valuelen;
      uint64_t actual_keylen;
      uint64_t actual_valuelen;
      uint8_t *keybuffer = NULL;
      uint8_t *valuebuffer = NULL;

      rc = sparkey_logiter_next(self->iterator, self->reader);
      if (SPARKEY_SUCCESS != rc) {
        errmsg = strdup(sparkey_errstring(rc));
        return;
      }

      // no keys left
      if (SPARKEY_ITER_ACTIVE != sparkey_logiter_state(self->iterator)) {
        return;
      }

      wanted_keylen = sparkey_logiter_keylen(self->iterator);
      wanted_valuelen = sparkey_logiter_valuelen(self->iterator);

      // calloc to ensure \0s
      // +1 to account for trailing \0
      keybuffer = (uint8_t *) calloc(wanted_keylen + 1, 1);
      valuebuffer = (uint8_t *) calloc(wanted_valuelen + 1, 1);

      if (!keybuffer || !valuebuffer) {
        free(keybuffer);
        free(valuebuffer);
        errmsg = strdup("Unable to allocate memory");
        return;
      }

      rc = sparkey_logiter_fill_key(
          self->iterator
        , self->reader
        , wanted_keylen
        , keybuffer
        , &actual_keylen
      );
      if (SPARKEY_SUCCESS != rc) {
        delete keybuffer;
        delete valuebuffer;
        errmsg = strdup(sparkey_errstring(rc));
        return;
      }
      // TODO assert(actual_keylen == wanted_keylen)

      rc = sparkey_logiter_fill_value(
          self->iterator
        , self->reader
        , wanted_valuelen
        , valuebuffer
        , &actual_valuelen
      );
      if (SPARKEY_SUCCESS != rc) {
        delete keybuffer;
        delete valuebuffer;
        errmsg = strdup(sparkey_errstring(rc));
        return;
      }
      // TODO assert(actual_keylen == wanted_keylen)

      type = sparkey_logiter_type(self->iterator);
      key = (char *) keybuffer;
      value = (char *) valuebuffer;
    };

    void
    HandleOKCallback() {
      v8::Local<v8::Value> argv[4];
      argv[0] = NanNewLocal<v8::Value>(v8::Null());
      if (key && value) {
        argv[1] = NanNewLocal<v8::String>(v8::String::New(key));
        argv[2] = NanNewLocal<v8::String>(v8::String::New(value));
        argv[3] = NanNewLocal<v8::Number>(v8::Number::New(type));
      } else {
        // no keys left
        for (int i = 1; i < 4; i++) {
          argv[i] = NanNewLocal<v8::Value>(v8::Null());
        }
      }
      callback->Call(4, argv);
    }

  private:
    LogReaderIterator *self;
    char *key;
    char *value;
    sparkey_entry_type type;
};

LogReaderIterator::LogReaderIterator() {}

LogReaderIterator::~LogReaderIterator() {}

/*
 * Methods exposed to v8.
 */

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

} // namespace sparkey
