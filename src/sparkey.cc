
#include <node.h>
#include <v8.h>
#include <sparkey.h>
#include "log-writer/writer.h"
#include "log-reader/reader.h"
#include "log-reader/iterator.h"
#include "hash-reader/reader.h"
#include "hash-reader/iterator.h"
#include "hash.h"

void
InitSparkey(v8::Handle<v8::Object> exports) {
  sparkey::LogWriter::Init(exports);
  sparkey::LogReader::Init(exports);
  sparkey::LogReaderIterator::Init();
  sparkey::HashReader::Init(exports);
  sparkey::HashIterator::Init();
  sparkey::InitHash(exports);

  // export iterator entry types
  exports->Set(
      NanSymbol("SPARKEY_ENTRY_PUT")
    , v8::Number::New(SPARKEY_ENTRY_PUT)
  );
  exports->Set(
      NanSymbol("SPARKEY_ENTRY_DELETE")
    , v8::Number::New(SPARKEY_ENTRY_DELETE)
  );
}

NODE_MODULE(sparkey, InitSparkey)
