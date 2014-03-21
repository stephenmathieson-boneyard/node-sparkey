
#ifndef NODE_SPARKEY_LOG_WRITER_H
#define NODE_SPARKEY_LOG_WRITER_H 1

#include <node.h>
#include <nan.h>
#include <sparkey.h>

namespace sparkey {
  class LogWriter : public node::ObjectWrap {
    public:
      char *path;
      sparkey_logwriter *writer;

      static v8::Persistent<v8::FunctionTemplate> constructor;
      static void Init(v8::Handle<v8::Object>);

    private:
      LogWriter();
      ~LogWriter();

      static v8::Handle<v8::Value> New(const v8::Arguments &);

      static NAN_METHOD(Open);
      static NAN_METHOD(OpenSync);

      static NAN_METHOD(Put);
      static NAN_METHOD(PutSync);

      static NAN_METHOD(Close);
      static NAN_METHOD(CloseSync);
  };
}

#endif
