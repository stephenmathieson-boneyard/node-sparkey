
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

    after(function () {
      rm(log);
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
});
