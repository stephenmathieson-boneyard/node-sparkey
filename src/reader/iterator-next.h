
#ifndef NODE_SPARKEY_LOG_READER_ITERATOR_NEXT_H
#define NODE_SPARKEY_LOG_READER_ITERATOR_NEXT_H 1

#include <nan.h>
#include "iterator.h"

namespace sparkey {
  void
  LogReaderIteratorNext(LogReaderIterator *, NanCallback *);
}

#endif
