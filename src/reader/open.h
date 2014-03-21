
#ifndef NODE_SPARKEY_LOG_READER_OPEN_H
#define NODE_SPARKEY_LOG_READER_OPEN_H 1

#include <nan.h>
#include "reader.h"

namespace sparkey {
  void
  LogReaderOpen(LogReader *, NanCallback *);

  void
  LogReaderOpenSync(LogReader *);
}

#endif
