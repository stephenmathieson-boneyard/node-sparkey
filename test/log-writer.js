
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
