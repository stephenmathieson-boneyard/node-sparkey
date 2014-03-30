
#include <stdlib.h>
#include <node.h>
#include <nan.h>
#include "reader.h"
#include "iterator.h"

namespace sparkey {

v8::Persistent<v8::FunctionTemplate> HashIterator::constructor;

class HashIteratorNextWorker : public NanAsyncWorker {
  public:
    HashIteratorNextWorker(
          HashIterator *self
        , NanCallback *callback
      ) : NanAsyncWorker(callback), self(self) {
      key = NULL;
      value = NULL;
    }

    ~HashIteratorNextWorker() {
      if (key) delete key;
      if (value) delete value;
    }

    void
    Execute() {
      sparkey_returncode rc;
      sparkey_logreader *logreader;
      uint64_t wanted_keylen;
      uint64_t wanted_valuelen;
      uint64_t actual_keylen;
      uint64_t actual_valuelen;
      uint8_t *keybuffer = NULL;
      uint8_t *valuebuffer = NULL;

      // TODO: is this safe enough?
      logreader = sparkey_hash_getreader(self->reader);

      rc = sparkey_logiter_hashnext(self->iterator, self->reader);
      if (SPARKEY_SUCCESS != rc) {
        return SetError(rc);
      }

      // no keys left
      if (SPARKEY_ITER_ACTIVE != sparkey_logiter_state(self->iterator)) {
        return;
      }

      wanted_keylen = sparkey_logiter_keylen(self->iterator);
      wanted_valuelen = sparkey_logiter_valuelen(self->iterator);

      // calloc/+1 to account for trailing \0
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
        , logreader
        , wanted_keylen
        , keybuffer
        , &actual_keylen
      );
      if (SPARKEY_SUCCESS != rc) {
        free(keybuffer);
        free(valuebuffer);
        return SetError(rc);
      }
      // TODO assert(actual_keylen == wanted_keylen)

      rc = sparkey_logiter_fill_value(
          self->iterator
        , logreader
        , wanted_valuelen
        , valuebuffer
        , &actual_valuelen
      );
      if (SPARKEY_SUCCESS != rc) {
        free(keybuffer);
        free(valuebuffer);
        return SetError(rc);
      }
      // TODO assert(actual_keylen == wanted_keylen)

      type = sparkey_logiter_type(self->iterator);
      key = (char *) keybuffer;
      value = (char *) valuebuffer;
    }

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
    HashIterator *self;
    char *key;
    char *value;
    sparkey_entry_type type;

    void
    SetError(sparkey_returncode rc) {
      errmsg = strdup(sparkey_errstring(rc));
    }
};

class HashIteratorGetWorker : public NanAsyncWorker {
  public:
    HashIteratorGetWorker(
        HashIterator *self
      , const char *key
      , NanCallback *callback
    ) : NanAsyncWorker(callback), self(self), key(key) {
      value = NULL;
    }

    ~HashIteratorGetWorker() {
      if (value) delete value;
    }

    void
    Execute() {
      sparkey_returncode rc;
      sparkey_logreader *logreader;
      size_t keysize = strlen(key);
      uint64_t wanted;
      uint64_t actual;
      uint8_t *buffer = NULL;

      // TODO: is this safe enough?
      logreader = sparkey_hash_getreader(self->reader);

      rc = sparkey_hash_get(
          self->reader
        , (uint8_t *) key
        , keysize
        , self->iterator
      );
      if (SPARKEY_SUCCESS != rc) {
        return SetError(rc);
      }

      // key not found
      if (SPARKEY_ITER_ACTIVE != sparkey_logiter_state(self->iterator)) {
        return;
      }

      // get size of value
      wanted = sparkey_logiter_valuelen(self->iterator);

      // calloc/1 to account for trailing \0
      if (!(buffer = (uint8_t *) calloc(wanted + 1, 1))) {
        errmsg = strdup("Unable to allocate memory");
        return;
      }

      rc = sparkey_logiter_fill_value(
          self->iterator
        , logreader
        , wanted
        , buffer
        , &actual
      );
      if (SPARKEY_SUCCESS != rc) {
        free(buffer);
        return SetError(rc);
      }
      // TODO assert(actual_keylen == wanted_keylen)

      value = (char *) buffer;
    }

    void
    HandleOKCallback() {
      v8::Local<v8::Value> err = NanNewLocal<v8::Value>(v8::Null());
      v8::Local<v8::Value> val;
      if (value) {
        val = NanNewLocal<v8::String>(v8::String::New(value));
      } else {
        val = NanNewLocal<v8::Value>(v8::Null());
      }
      v8::Local<v8::Value> argv[2] = { err, val };
      callback->Call(2, argv);
    }

  private:
    void
    SetError(sparkey_returncode rc) {
      errmsg = strdup(sparkey_errstring(rc));
    }

    HashIterator *self;
    const char *key;
    char *value;
};


/**
 * Skip worker.
 */

class HashIteratorSkipWorker : public NanAsyncWorker {
  public:
    HashIteratorSkipWorker(
        HashIterator *self
      , int number
      , NanCallback *callback
    ) : NanAsyncWorker(callback), self(self), number(number) {}

    void
    Execute() {
      // see https://github.com/spotify/sparkey/issues/25
      for (int i = 0; i < number; i++) {
        sparkey_returncode rc = sparkey_logiter_hashnext(
            self->iterator
          , self->reader
        );
        if (SPARKEY_SUCCESS != rc) {
          errmsg = strdup(sparkey_errstring(rc));
          return;
        }
      }
    }

  private:
    HashIterator *self;
    int number;
};

HashIterator::HashIterator() {}

HashIterator::~HashIterator() {
  if (iterator) sparkey_logiter_close(&iterator);
}

void
HashIterator::Init() {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(
    HashIterator::New
  );
  NanAssignPersistent(v8::FunctionTemplate, constructor, tpl);
  tpl->SetClassName(NanSymbol("HashIterator"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(tpl, "next", Next);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get", Get);
  NODE_SET_PROTOTYPE_METHOD(tpl, "skip", Skip);
  NODE_SET_PROTOTYPE_METHOD(tpl, "end", End);
}

v8::Local<v8::Object>
HashIterator::NewInstance(v8::Local<v8::Object> reader) {
  NanScope();
  v8::Local<v8::Object> instance;
  v8::Local<v8::FunctionTemplate> c = NanPersistentToLocal(
    constructor
  );
  v8::Handle<v8::Value> argv[1] = { reader };
  instance = c->GetFunction()->NewInstance(1, argv);
  return instance;
}

NAN_METHOD(HashIterator::New) {
  NanScope();
  HashReader *reader = ObjectWrap::Unwrap<HashReader>(
    args[0]->ToObject()
  );
  HashIterator *self = new HashIterator;
  self->reader = reader->hash_reader;
  sparkey_returncode rc;
  rc = sparkey_logiter_create(
      &self->iterator
    , sparkey_hash_getreader(self->reader)
  );
  if (SPARKEY_SUCCESS != rc) {
    NanThrowError(sparkey_errstring(rc));
  }
  self->Wrap(args.This());
  NanReturnValue(args.This());
}

NAN_METHOD(HashIterator::Next) {
  NanScope();
  HashIterator *self = ObjectWrap::Unwrap<HashIterator>(
    args.This()
  );
  v8::Local<v8::Function> fn = args[0].As<v8::Function>();
  HashIteratorNextWorker *worker = new HashIteratorNextWorker(
      self
    , new NanCallback(fn)
  );
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(HashIterator::Get) {
  NanScope();
  size_t keysize;
  HashIterator *self = ObjectWrap::Unwrap<HashIterator>(
    args.This()
  );
  v8::Local<v8::Function> fn = args[1].As<v8::Function>();
  HashIteratorGetWorker *worker = new HashIteratorGetWorker(
      self
    , NanCString(args[0], &keysize)
    , new NanCallback(fn)
  );
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(HashIterator::Skip) {
  NanScope();
  HashIterator *self = ObjectWrap::Unwrap<HashIterator>(
    args.This()
  );
  int number = args[0]->NumberValue();
  v8::Local<v8::Function> fn = args[1].As<v8::Function>();
  HashIteratorSkipWorker *worker = new HashIteratorSkipWorker(
      self
    , number
    , new NanCallback(fn)
  );
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

NAN_METHOD(HashIterator::End) {
  NanScope();
  HashIterator *self = ObjectWrap::Unwrap<HashIterator>(
    args.This()
  );
  sparkey_logiter_close(&self->iterator);
  NanReturnUndefined();
}


} // namespace sparkey
