
#include <node.h>
#include <v8.h>
#include "writer/writer.h"
#include "reader/reader.h"
#include "reader/iterator.h"

void
InitSparkey(v8::Handle<v8::Object> exports) {
  sparkey::LogWriter::Init(exports);
  sparkey::LogReader::Init(exports);
  sparkey::LogReaderIterator::Init();
}

NODE_MODULE(sparkey, InitSparkey)
