
#ifndef NODE_SPARKEY_LOG_WRITER_OPEN_H
#define NODE_SPARKEY_LOG_WRITER_OPEN_H 1

#include <nan.h>
#include "writer.h"

namespace sparkey {
  void
  LogWriterOpen(LogWriter *, sparkey_compression_type, int, NanCallback *);

  void
  LogWriterOpenSync(LogWriter *, sparkey_compression_type, int);
}

#endif
