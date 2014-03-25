
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "writer.h"
#include "workers.h"

namespace sparkey {
  v8::Persistent<v8::FunctionTemplate> LogWriter::constructor;

  LogWriter::LogWriter() {
    writer = NULL;
  };
  LogWriter::~LogWriter() {
    delete path;
  };

  void
  LogWriter::Init(v8::Handle<v8::Object> exports) {
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(
      LogWriter::New
    );
    NanAssignPersistent(v8::FunctionTemplate, constructor, tpl);

    tpl->SetClassName(NanSymbol("LogWriter"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(tpl, "put", Put);
    NODE_SET_PROTOTYPE_METHOD(tpl, "putSync", PutSync);

    NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
    NODE_SET_PROTOTYPE_METHOD(tpl, "closeSync", CloseSync);

    NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
    NODE_SET_PROTOTYPE_METHOD(tpl, "openSync", OpenSync);

    NODE_SET_PROTOTYPE_METHOD(tpl, "append", Append);
    NODE_SET_PROTOTYPE_METHOD(tpl, "appendSync", AppendSync);

    exports->Set(NanSymbol("LogWriter"), constructor->GetFunction());
  }

  NAN_METHOD(LogWriter::New) {
    NanScope();
    LogWriter *self = new LogWriter;
    size_t pathsize;
    self->path = NanCString(args[0], &pathsize);
    self->Wrap(args.This());
    NanReturnValue(args.This());
  }

  NAN_METHOD(LogWriter::Open) {
    NanScope();
    LogWriter *self = node::ObjectWrap::Unwrap<LogWriter>(args.This());

    v8::Local<v8::Object> options;
    v8::Local<v8::Function> fn;
    if (2 == args.Length()) {
      options = v8::Local<v8::Object>::Cast(args[0]);
      fn = args[1].As<v8::Function>();
    } else {
      fn = args[0].As<v8::Function>();
    }

    bool use_snappy = NanBooleanOptionValue(
        options
      , NanSymbol("useSnappy")
      , true
    );
    sparkey_compression_type compression_type = use_snappy
      ? SPARKEY_COMPRESSION_SNAPPY
      : SPARKEY_COMPRESSION_NONE;
    // TODO this really should be an option
    int block_size = 10;
    WriterOpenWorker *worker = new WriterOpenWorker(
        self
      , block_size
      , compression_type
      , new NanCallback(fn)
    );
    NanAsyncQueueWorker(worker);
    NanReturnUndefined();
  }

  NAN_METHOD(LogWriter::OpenSync) {
    NanScope();
    LogWriter *self = node::ObjectWrap::Unwrap<LogWriter>(args.This());
    v8::Local<v8::Object> options = v8::Local<v8::Object>::Cast(args[0]);
    bool use_snappy = NanBooleanOptionValue(
        options
      , NanSymbol("useSnappy")
      , true
    );
    sparkey_compression_type compression_type = use_snappy
      ? SPARKEY_COMPRESSION_SNAPPY
      : SPARKEY_COMPRESSION_NONE;
    // TODO option
    int block_size = 10;
    sparkey_returncode rc = sparkey_logwriter_create(
        &self->writer
      , self->path
      , compression_type
      , block_size
    );
    if (SPARKEY_SUCCESS != rc) {
      NanThrowError(sparkey_errstring(rc));
    }
    NanReturnUndefined();
  }

  NAN_METHOD(LogWriter::Append) {
    NanScope();
    LogWriter *self = node::ObjectWrap::Unwrap<LogWriter>(args.This());
    v8::Local<v8::Function> fn = args[0].As<v8::Function>();
    WriterAppendWorker *worker = new WriterAppendWorker(
        self
      , new NanCallback(fn)
    );
    NanAsyncQueueWorker(worker);
    NanReturnUndefined();
  }

  NAN_METHOD(LogWriter::AppendSync) {
    NanScope();
    LogWriter *self = node::ObjectWrap::Unwrap<LogWriter>(args.This());
    sparkey_returncode rc = sparkey_logwriter_append(
        &self->writer
      , self->path
    );
    if (SPARKEY_SUCCESS != rc) {
      NanThrowError(sparkey_errstring(rc));
    }
    NanReturnUndefined();
  }

  NAN_METHOD(LogWriter::Put) {
    NanScope();
    LogWriter *self = node::ObjectWrap::Unwrap<LogWriter>(args.This());
    size_t keysize;
    size_t valuesize;
    char *key = NanCString(args[0], &keysize);
    char *value = NanCString(args[1], &valuesize);
    v8::Local<v8::Function> fn = args[2].As<v8::Function>();
    WriterPutWorker *worker = new WriterPutWorker(
        self
      , key
      , keysize
      , value
      , valuesize
      , new NanCallback(fn)
    );
    NanAsyncQueueWorker(worker);
    NanReturnUndefined();
  }

  NAN_METHOD(LogWriter::PutSync) {
    NanScope();
    LogWriter *self = node::ObjectWrap::Unwrap<LogWriter>(args.This());
    size_t keysize;
    size_t valuesize;
    char *key = NanCString(args[0], &keysize);
    char *value = NanCString(args[1], &valuesize);
    sparkey_returncode rc = sparkey_logwriter_put(
        self->writer
      , keysize
      , (uint8_t *) key
      , valuesize
      , (uint8_t *) value
    );

    delete key;
    delete value;

    if (SPARKEY_SUCCESS != rc) {
      NanThrowError(sparkey_errstring(rc));
    }
    NanReturnUndefined();
  }

  NAN_METHOD(LogWriter::Close) {
    NanScope();
    LogWriter *self = node::ObjectWrap::Unwrap<LogWriter>(args.This());
    v8::Local<v8::Function> fn = args[0].As<v8::Function>();
    WriterCloseWorker *worker = new WriterCloseWorker(
        self
      , new NanCallback(fn)
    );
    NanAsyncQueueWorker(worker);
    NanReturnUndefined();
  }

  NAN_METHOD(LogWriter::CloseSync) {
    NanScope();
    LogWriter *self = node::ObjectWrap::Unwrap<LogWriter>(args.This());
    sparkey_returncode rc = sparkey_logwriter_close(&self->writer);
    if (SPARKEY_SUCCESS != rc) {
      NanThrowError(sparkey_errstring(rc));
    }
    NanReturnUndefined();
  }
}
