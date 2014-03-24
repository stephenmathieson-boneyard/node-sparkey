
#include <node.h>
#include <v8.h>
#include "writer/writer.h"
#include "reader/reader.h"
#include "reader/iterator.h"
#include "hash.h"

void
InitSparkey(v8::Handle<v8::Object> exports) {
  sparkey::LogWriter::Init(exports);
  sparkey::LogReader::Init(exports);
  sparkey::InitHash(exports);
  sparkey::LogReaderIterator::Init();
  exports->Set(
      NanSymbol("ITERATOR_ENTRY_PUT")
    , v8::Number::New(sparkey::ITERATOR_ENTRY_PUT)
  );
  exports->Set(
      NanSymbol("ITERATOR_ENTRY_DELETE")
    , v8::Number::New(sparkey::ITERATOR_ENTRY_DELETE)
  );
}

NODE_MODULE(sparkey, InitSparkey)
