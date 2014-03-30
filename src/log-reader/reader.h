
#ifndef NODE_SPARKEY_LOG_READER_H
#define NODE_SPARKEY_LOG_READER_H 1

#include <node.h>
#include <nan.h>
#include <sparkey.h>

namespace sparkey {

class LogReader : public node::ObjectWrap {
  public:
    char *path;
    sparkey_logreader *reader;
    sparkey_logiter *iterator;

    static v8::Persistent<v8::FunctionTemplate> constructor;

    sparkey_returncode
    OpenReader();

    void
    CloseReader();

    static void
    Init(v8::Handle<v8::Object>);

  private:
    LogReader();
    ~LogReader();

    bool is_open;
    int block_size;

    static NAN_METHOD(New);
    static NAN_METHOD(Open);
    static NAN_METHOD(OpenSync);
    static NAN_METHOD(Close);
    static NAN_METHOD(CloseSync);
    static NAN_METHOD(GetBlockSize);
    static NAN_METHOD(NewIterator);
};

} // namespace sparkey

#endif
