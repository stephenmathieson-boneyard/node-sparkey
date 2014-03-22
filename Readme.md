
# sparkey

  Node binding for [Sparkey](https://github.com/spotify/sparkey).

## API

### sparkey.LogReader(path)

  Create a `LogReader` for `path`.

#### LogReader#open(fn)

  Open the `LogReader`, invoking `fn(err)`.

#### LogReader#openSync()

  Synchronously open the `LogReader`.

#### LogReader#close(fn)

  Close the `LogReader`, invoking `fn(err)`.

#### LogReader#closeSync()

  Synchronously close the `LogReader`.

#### LogReader#iterator()

  Create and return a `LogIterator`.

#### LogIterator#next(fn)

  Get the next `key` and `value` pair in the log.  Invokes `fn(err, key, value)` or `fn(err, null, null)` if no keys remain.

#### LogIterator#end()

  End/destroy the `LogIterator`.

### sparkey.LogWriter(path)

  Create a `LogWriter` for `path`.  If no log exists at `path`, it will be created.

#### LogWriter#open(fn)

  Open the `LogWriter`, invoking `fn(err)`.

#### LogWriter#openSync()

  Synchronously open the `LogWriter`.

#### LogWriter#append(fn)

  Open the `LogWriter` for appending, invoking `fn(err)`.

  Will error if the log does not exist.

#### LogWriter#appendSync()

  Synchronously open the `LogWriter` for append.

  Will throw if the log does not exist.

#### LogWriter#close(fn)

  Close the `LogWriter`, invoking `fn(err)`.

#### LogWriter#closeSync()

  Synchronously close the `LogWriter`.

#### LogWriter#put(key, value, fn)

  Put `key = value`, invoking `fn(err)`.

#### LogWriter#putSync(key, value)

  Synchronously put `key = value`.

### sparkey.hash(log, hash, fn)

  Convert the given `log` to a `hash`, invoking `fn(err)`.

### sparkey.hashSync(log, hash)

  Synchronously convert the given `log` to a `hash`.

## License

  MIT
