
#ifndef NODE_SPARKEY_LOG_READER_ITERATOR_H
#define NODE_SPARKEY_LOG_READER_ITERATOR_H 1

#include <node.h>
#include <nan.h>
#include "reader.h"

namespace sparkey {
  class LogReaderIterator : public node::ObjectWrap {
    public:
      static v8::Persistent<v8::FunctionTemplate> constructor;
      sparkey_logreader *reader;
      sparkey_logiter *iterator;
      static void Init();
      static v8::Local<v8::Object> NewInstance(v8::Local<v8::Object>);

    private:
      LogReaderIterator();
      ~LogReaderIterator();

      static v8::Handle<v8::Value> New(const v8::Arguments &);
      static NAN_METHOD(End);
      static NAN_METHOD(Next);
      static NAN_METHOD(Skip);
      static NAN_METHOD(IsActive);
  };
}

#endif
