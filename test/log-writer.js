
var sparkey = require('..');
var LogReader = sparkey.LogReader;
var LogWriter = sparkey.LogWriter;
var path = require('path');
var assert = require('assert');
var fs = require('fs');
var rm = fs.unlinkSync;

var log = path.join(__dirname, 'tmp.spl');

describe('LogWriter', function () {
  describe('#open', function () {
    after(function () {
      rm(log);
    });

    it('should create a new log', function (done) {
      try { rm(log); } catch (e) {}
      var writer = new LogWriter(log);
      writer.open(function (err) {
        assert.ifError(err);
        assert(fs.existsSync(log));
        writer.close(done);
      });
    });

    it('should open an existing log', function (done) {
      assert(fs.existsSync(log));
      var writer = new LogWriter(log);
      writer.open(function (err) {
        assert.ifError(err);
        writer.close(done);
      });
    });

    it('should allow block size specification', function (done) {
      var writer = new LogWriter(log);
      writer.open({ blockSize: 1000 }, function (err) {
        assert.ifError(err);
        writer.close(done);
      });
    });
  });

  describe('#openSync', function () {
    after(function () {
      rm(log);
    });

    it('should create a new log', function () {
      try { rm(log); } catch (e) {}
      var writer = new LogWriter(log);
      writer.openSync();
      writer.closeSync();
    });

    it('should open an existing log', function () {
      assert(fs.existsSync(log));
      var writer = new LogWriter(log);
      writer.openSync();
      writer.closeSync();
    });

    it('should allow block size specification', function () {
      assert(fs.existsSync(log));
      var writer = new LogWriter(log);
      writer.openSync({ blockSize: 1000 });
      writer.closeSync();
    });
  });

  describe('#append', function () {
    after(function () {
      rm(log);
    });

    it('should not create a new log', function (done) {
      try { rm(log); } catch (e) {}
      var writer = new LogWriter(log);
      writer.append(function (err) {
        assert(err);
        assert(!fs.existsSync(log));
        done();
      });
    });

    it('should open an existing log for appending', function (done) {
      try { rm(log); } catch (e) {}

      // create dummy log
      var writer1 = new LogWriter(log);
      writer1.openSync();
      writer1.putSync('key_0', 'value_0');
      writer1.closeSync();

      // append dummy log
      var writer2 = new LogWriter(log);
      writer2.append(function (err) {
        assert.ifError(err);
        writer2.putSync('key_1', 'value_1');
        writer2.close(done);
      });
    });
  });

  describe('#appendSync', function () {
    after(function () {
      rm(log);
    });

    it('should not create a new log', function () {
      try { rm(log); } catch (e) {}

      var err;
      var writer = new LogWriter(log);
      try {
        writer.appendSync();
      } catch (e) {
        err = e;
      }
      assert(err);
    });

    it('should open an existing log for appending', function () {
      try { rm(log); } catch (e) {}

      // create dummy log
      var writer1 = new LogWriter(log);
      writer1.openSync();
      writer1.putSync('key_0', 'value_0');
      writer1.closeSync();

      // append dummy log
      var writer2 = new LogWriter(log);
      writer2.appendSync();
      writer2.putSync('key_1', 'value_1');
      writer2.closeSync();
    });
  });

  describe('#put', function () {
    beforeEach(function () {
      try { rm(log); } catch (e) {}
    });

    afterEach(function () {
      try { rm(log); } catch (e) {}
    });

    it('should put `key` = `value`', function (done) {
      var writer = new LogWriter(log);
      writer.open(function (err) {
        assert.ifError(err);
        writer.put('key', 'value', function (err) {
          assert.ifError(err);
          writer.closeSync();

          var reader = new LogReader(log);
          reader.openSync();

          var iterator = reader.iterator();
          iterator.next(function (err, key, value) {
            assert.ifError(err);
            assert.equal('key', key);
            assert.equal('value', value);
            iterator.end();
            reader.close(done);
          });
        });
      });
    });

    it('should fail to write to an unopen writer', function (done) {
      var writer = new LogWriter(log);
      writer.put('foo', 'bar', function (err) {
        assert(err);
        done();
      });
    });
  });

  describe('#putSync', function () {
    beforeEach(function () {
      try { rm(log); } catch (e) {}
    });

    after(function () {
      rm(log);
    });

    it('should put `key` = `value`', function (done) {
      var writer = new LogWriter(log);
      writer.open(function (err) {
        assert.ifError(err);
        writer.putSync('key', 'value');
        assert.ifError(err);
        writer.closeSync();
        var reader = new LogReader(log);
        reader.openSync();
        var iterator = reader.iterator();
        iterator.next(function (err, key, value) {
          assert.ifError(err);
          assert.equal('key', key);
          assert.equal('value', value);
          iterator.end();
          reader.close(done);
        });
      });
    });
  });

  describe('#delete', function () {
    it('should delete a key form the log', function (done) {
      var writer = new LogWriter(log);
      writer.openSync();
      writer.putSync('key', 'value');
      writer.delete('key', function (err) {
        assert.ifError(err);
        writer.closeSync();

        var reader = new LogReader(log);
        reader.openSync();
        var iterator = reader.iterator();
        // jump over our put
        iterator.skip(1, function (err) {
          assert.ifError(err);
          iterator.next(function (err, key, value, type) {
            assert.ifError(err);
            assert.equal('key', key);
            assert(!value);
            assert.equal(sparkey.SPARKEY_ENTRY_DELETE, type);
            reader.close(done);
          });
        });
      });
    });
  });

  describe('#deleteSync', function () {
    it('should delete a key form the log', function (done) {
      var writer = new LogWriter(log);
      writer.openSync();
      writer.putSync('key', 'value');
      writer.deleteSync('key');
      writer.closeSync();

      var reader = new LogReader(log);
      reader.openSync();
      var iterator = reader.iterator();
      // jump over our put
      iterator.skip(1, function (err) {
        assert.ifError(err);
        iterator.next(function (err, key, value, type) {
          assert.ifError(err);
          assert.equal('key', key);
          assert(!value);
          assert.equal(sparkey.SPARKEY_ENTRY_DELETE, type);
          reader.close(done);
        });
      });
    });
  });

  describe('#flush', function () {
    it('should flush a log, allowing it to be read', function (done) {
      var writer = new LogWriter(log);
      writer.openSync();
      for (var i = 0; i < 5; i++) {
        writer.putSync('key_' + i, 'value_' + i);
      }

      writer.flush(function (err) {
        assert.ifError(err);
        var reader = new LogReader(log);
        reader.openSync();
        var it = reader.iterator();
        var i = 0;

        it.next(next);

        function next(err, key, value, type) {
          assert.ifError(err);

          if (5 == i) {
            assert.equal(null, key);
            assert.equal(null, value);
            assert.equal(null, type);
            reader.closeSync();
            return writer.close(done);
          }

          assert.equal(sparkey.SPARKEY_ENTRY_PUT, type);
          assert.equal('key_' + i, key);
          assert.equal('value_' + i, value);
          i++;
          it.next(next);
        }
      });
    });
  });

  describe('#flushSync', function () {
    it('should flush a log, allowing it to be read', function (done) {
      var writer = new LogWriter(log);
      writer.openSync();
      for (var i = 0; i < 5; i++) {
        writer.putSync('key_' + i, 'value_' + i);
      }

      writer.flushSync();
      var reader = new LogReader(log);
      reader.openSync();
      var it = reader.iterator();
      var i = 0;

      it.next(next);

      function next(err, key, value, type) {
        assert.ifError(err);

        if (5 == i) {
          assert.equal(null, key);
          assert.equal(null, value);
          assert.equal(null, type);
          reader.closeSync();
          return writer.close(done);
        }

        assert.equal(sparkey.SPARKEY_ENTRY_PUT, type);
        assert.equal('key_' + i, key);
        assert.equal('value_' + i, value);
        i++;
        it.next(next);
      }
    });
  });
});
