
#ifndef NODE_SPARKEY_HASH_H
#define NODE_SPARKEY_HASH_H 1

namespace sparkey {
  void
  InitHash(v8::Handle<v8::Object>);

  NAN_METHOD(Hash);

  NAN_METHOD(HashSync);
}

#endif
