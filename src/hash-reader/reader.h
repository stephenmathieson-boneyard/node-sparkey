
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

    /**
     * Internal method for opening the hash reader.
     */

    sparkey_returncode
    OpenReader();

    /**
     * Internal method for closing the hash reader.
     */

    void
    CloseReader();

    /**
     * Init our class for v8.
     */

    static void
    Init(v8::Handle<v8::Object>);

  private:

    /**
     * Is open flag.
     */

    bool is_open;

    HashReader();
    ~HashReader();

    /**
     * Methods exposed to v8.
     */

    static NAN_METHOD(New);
    static NAN_METHOD(Open);
    static NAN_METHOD(OpenSync);
    static NAN_METHOD(Close);
    static NAN_METHOD(CloseSync);
    static NAN_METHOD(Count);
    static NAN_METHOD(NewIterator);
};

} // namespace sparkey

#endif
