
#ifndef NODE_SPARKEY_LOG_WRITER_PUT_H
#define NODE_SPARKEY_LOG_WRITER_PUT_H 1

#include <nan.h>
#include "writer.h"

namespace sparkey {
  void
  LogWriterPut(LogWriter *, char *, size_t, char *, size_t, NanCallback *);

  void
  LogWriterPutSync(LogWriter *, char *, size_t, char *, size_t);
}

#endif
