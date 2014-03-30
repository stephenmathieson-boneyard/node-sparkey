
#ifndef NODE_SPARKEY_HASH_READER_H
#define NODE_SPARKEY_HASH_READER_H 1

#include <node.h>
#include <nan.h>
#include <sparkey.h>

namespace sparkey {
  class HashReader : public node::ObjectWrap {
    public:
      const char *hashfile;
      const char *logfile;
      sparkey_hashreader *hash_reader;
      sparkey_logreader *log_reader;

      static v8::Persistent<v8::FunctionTemplate> constructor;

      static void
      Init(v8::Handle<v8::Object>);

    private:
      HashReader();
      ~HashReader();

      static NAN_METHOD(New);
      static NAN_METHOD(Open);
      static NAN_METHOD(Close);
      static NAN_METHOD(Count);
      static NAN_METHOD(NewIterator);
  };
} // namespace sparkey

#endif
