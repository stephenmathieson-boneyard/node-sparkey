
#include <stdlib.h>
#include <node.h>
#include <nan.h>
#include <sparkey.h>
#include "iterator.h"
#include "iterator-next.h"

namespace sparkey {
  class LogReaderIteratorNextWorker : public NanAsyncWorker {
    public:
      LogReaderIteratorNextWorker(
            LogReaderIterator *self
          , NanCallback *callback
        ) : NanAsyncWorker(callback)
          , self(self) {
            key = NULL;
            value = NULL;
          }

      ~LogReaderIteratorNextWorker() {
        free(key);
        free(value);
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

        rc = sparkey_logiter_fill_key(self->iterator
          , self->reader
          , wanted_keylen
          , keybuffer
          , &actual_keylen);
        if (SPARKEY_SUCCESS != rc) {
          free(keybuffer);
          free(valuebuffer);
          errmsg = strdup(sparkey_errstring(rc));
          return;
        }

        rc = sparkey_logiter_fill_value(self->iterator
          , self->reader
          , wanted_valuelen
          , valuebuffer
          , &actual_valuelen);
        if (SPARKEY_SUCCESS != rc) {
          free(keybuffer);
          free(valuebuffer);
          errmsg = strdup(sparkey_errstring(rc));
          return;
        }

        key = (char *) keybuffer;
        value = (char *) valuebuffer;
      }

      void
      HandleOKCallback() {
        if (key && value) {
          v8::Local<v8::Value> argv[3] = {
              v8::Local<v8::Value>::New(v8::Null())
            , v8::Local<v8::Value>::New(v8::String::New(key))
            , v8::Local<v8::Value>::New(v8::String::New(value))
          };
          callback->Call(3, argv);
          return;
        }

        // no keys left
        v8::Local<v8::Value> argv[3] = {
            v8::Local<v8::Value>::New(v8::Null())
          , v8::Local<v8::Value>::New(v8::Null())
          , v8::Local<v8::Value>::New(v8::Null())
        };
        callback->Call(3, argv);
        return;
      }

    private:
      LogReaderIterator *self;
      char *key;
      char *value;
  };

  void
  LogReaderIteratorNext(LogReaderIterator *self, NanCallback *callback) {
    LogReaderIteratorNextWorker *worker = new LogReaderIteratorNextWorker(
        self
      , callback
    );
    NanAsyncQueueWorker(worker);
  }
}
