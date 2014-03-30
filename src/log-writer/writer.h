
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

    /**
     * Internal method for opening the writer.
     */

    sparkey_returncode
    OpenWriter(int, sparkey_compression_type);

    /**
     * Internal method for opening the writer in append mode.
     */

    sparkey_returncode
    OpenWriterForAppending();

    /**
     * Internal method for writing to the writer.
     */

    sparkey_returncode
    Set(size_t, const char *, size_t, const char *);

    /**
     * Internal method for closing the writer.
     */

    sparkey_returncode
    CloseWriter();

    static v8::Persistent<v8::FunctionTemplate> constructor;
    static void Init(v8::Handle<v8::Object>);

  private:
    LogWriter();
    ~LogWriter();

    /**
     * Is open flag.
     */

    bool is_open;

    static NAN_METHOD(New);
    static NAN_METHOD(Open);
    static NAN_METHOD(OpenSync);
    static NAN_METHOD(Append);
    static NAN_METHOD(AppendSync);
    static NAN_METHOD(Put);
    static NAN_METHOD(PutSync);
    static NAN_METHOD(Close);
    static NAN_METHOD(CloseSync);
};

} // namespace sparkey

#endif
