
#ifndef NODE_SPARKEY_LOG_WRITER_CLOSE_H
#define NODE_SPARKEY_LOG_WRITER_CLOSE_H 1

#include <nan.h>
#include "writer.h"

namespace sparkey {
  void
  LogWriterClose(LogWriter *, NanCallback *);

  void
  LogWriterCloseSync(LogWriter *);
}

#endif
