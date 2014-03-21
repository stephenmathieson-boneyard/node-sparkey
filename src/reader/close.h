
#ifndef NODE_SPARKEY_LOG_READER_CLOSE_H
#define NODE_SPARKEY_LOG_READER_CLOSE_H 1

#include <nan.h>
#include "reader.h"

namespace sparkey {
  void
  LogReaderClose(LogReader *, NanCallback *);

  void
  LogReaderCloseSync(LogReader *);
}

#endif
