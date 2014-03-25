
#include <stdlib.h>
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "reader.h"
#include "iterator.h"
#include "workers.h"

namespace sparkey {
  LogReaderCloseWorker::LogReaderCloseWorker(
      LogReader *self
    , NanCallback *callback
  ) : NanAsyncWorker(callback), self(self) {}

  void
  LogReaderCloseWorker::Execute() {
    sparkey_logreader_close(&self->reader);
  }

  LogReaderOpenWorker::LogReaderOpenWorker(
      LogReader *self
    , NanCallback *callback
  ) : NanAsyncWorker(callback), self(self) {}

  void
  LogReaderOpenWorker::Execute() {
    sparkey_returncode rc = sparkey_logreader_open(&self->reader, self->path);
    if (SPARKEY_SUCCESS != rc) {
      // most obnixous part of NAN: they free `errmsg`, so
      // you've got to use a pointer you own :(
      errmsg = strdup(sparkey_errstring(rc));
    }
  }

  LogReaderIteratorNextWorker::LogReaderIteratorNextWorker(
      LogReaderIterator *self
    , NanCallback *callback
  ) : NanAsyncWorker(callback), self(self) {
    key = NULL;
    value = NULL;
  }

  LogReaderIteratorNextWorker::~LogReaderIteratorNextWorker() {
    free(key);
    free(value);
  }

  void
  LogReaderIteratorNextWorker::Execute() {
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
      free(keybuffer);
      free(valuebuffer);
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
      free(keybuffer);
      free(valuebuffer);
      errmsg = strdup(sparkey_errstring(rc));
      return;
    }
    // TODO assert(actual_keylen == wanted_keylen)

    type = sparkey_logiter_type(self->iterator);
    key = (char *) keybuffer;
    value = (char *) valuebuffer;
  }

  void
  LogReaderIteratorNextWorker::HandleOKCallback() {
    if (key && value) {
      v8::Local<v8::Value> argv[4] = {
          v8::Local<v8::Value>::New(v8::Null())
        , v8::Local<v8::Value>::New(v8::String::New(key))
        , v8::Local<v8::Value>::New(v8::String::New(value))
        , v8::Local<v8::Value>::New(v8::Number::New(type))
      };
      callback->Call(4, argv);
      return;
    }

    // no keys left
    v8::Local<v8::Value> argv[4] = {
        v8::Local<v8::Value>::New(v8::Null())
      , v8::Local<v8::Value>::New(v8::Null())
      , v8::Local<v8::Value>::New(v8::Null())
      , v8::Local<v8::Value>::New(v8::Null())
    };
    callback->Call(4, argv);
  }

  LogReaderIteratorSkipWorker::LogReaderIteratorSkipWorker(
      LogReaderIterator *self
    , int number
    , NanCallback *callback
  ) : NanAsyncWorker(callback), self(self), number(number) {}

  void
  LogReaderIteratorSkipWorker::Execute() {
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

}
