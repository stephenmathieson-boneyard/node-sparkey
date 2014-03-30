
#ifndef NODE_SPARKEY_HASH_ITERATOR_H
#define NODE_SPARKEY_HASH_ITERATOR_H 1

#include <node.h>
#include <nan.h>
#include "reader.h"

namespace sparkey {

class HashIterator : public node::ObjectWrap {
  public:
    static v8::Persistent<v8::FunctionTemplate> constructor;
    sparkey_hashreader *reader;
    sparkey_logiter *iterator;

    /**
     * Init the Iterator for v8.
     */

    static void
    Init();

    /**
     * Create a new instance of the Iterator.
     */

    static v8::Local<v8::Object>
    NewInstance(v8::Local<v8::Object>);

  private:
    HashIterator();
    ~HashIterator();

    /**
     * Create an instance (node).
     */

    static NAN_METHOD(New);

    /**
     * Get the next key/value/type.
     */

    static NAN_METHOD(Next);

    /**
     * Get a key's value.
     */

    static NAN_METHOD(Get);

    /**
     * Skip over a number of entries.
     */

    static NAN_METHOD(Skip);

    /**
     * End the iterator's cycle and destroy the iterator itself.
     */

    static NAN_METHOD(End);
};

} // namespace sparkey

#endif
