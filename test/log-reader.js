
var sparkey = require('..');
var TYPE_PUT = sparkey.SPARKEY_ENTRY_PUT;
var TYPE_DELETE = sparkey.SPARKEY_ENTRY_DELETE;
var LogReader = sparkey.LogReader;
var LogWriter = sparkey.LogWriter;
var path = require('path');
var assert = require('assert');
var rm = require('fs').unlinkSync;

var log = path.join(__dirname, 'tmp.spl');

describe('LogReader', function () {
  describe('#open', function () {
    beforeEach(function () {
      try { rm(log); } catch (e) {}
    });

    it('should open an existing log', function (done) {
      var writer = new LogWriter(log);
      writer.openSync();
      writer.putSync('foo', 'bar');
      writer.closeSync();

      var reader = new LogReader(log);
      reader.open(function (err) {
        assert.ifError(err);
        reader.close(done);
      });
    });

    it('should not create a new log', function (done) {
      var reader = new LogReader(log);
      reader.open(function (err) {
        assert(err);
        done();
      });
    });
  });

  describe('#openSync', function () {
    beforeEach(function () {
      try { rm(log); } catch (e) {}
    });

    it('should open an existing log', function () {
      var writer = new LogWriter(log);
      writer.openSync();
      writer.putSync('foo', 'bar');
      writer.closeSync();

      var reader = new LogReader(log);
      reader.openSync();
    });

    it('should not create a new log', function () {
      var reader = new LogReader(log);
      var err;
      try {
        reader.openSync();
      } catch (e) {
        err = e;
      }
      assert(err);
    });
  });

  describe('#iterator', function () {
    beforeEach(function () {
      try { rm(log); } catch (e) {}
      var writer = new LogWriter(log);
      writer.openSync();
      for (var i = 0; i < 100; i++) {
        writer.putSync('key' + i, 'value' + i);
      }
      writer.closeSync();
    });

    describe('#next', function () {
      it('should get the next key/value', function (done) {
        var reader = new LogReader(log);
        reader.open(function (err) {
          assert.ifError(err);
          var iterator = reader.iterator();
          iterator.next(function (err, key, value, type) {
            assert.ifError(err);
            assert.equal('key0', key);
            assert.equal('value0', value);
            assert.equal(TYPE_PUT, type);
            iterator.next(function (err, key, value, type) {
              assert.ifError(err);
              assert.equal('key1', key);
              assert.equal('value1', value);
              assert.equal(TYPE_PUT, type);
              iterator.end();
              reader.close(done);
            });
          });
        });
      });

      it('should provide NULLs when all keys have been seen', function (done) {
        var reader = new LogReader(log);
        reader.open(function (err) {
          assert.ifError(err);
          var iterator = reader.iterator();
          var count = 0;

          iterator.next(next);

          function next(err, key, value, type) {
            if (100 == count) {
              assert(null === key);
              assert(null === value);
              assert(null === type);
              iterator.end();
              return reader.close(done);
            }
            assert.equal('key' + count, key);
            assert.equal('value' + count, value);
            count++;
            iterator.next(next);
          }
        });
      });
    });

    describe('#skip', function () {
      it('should skip N keys in the log', function (done) {
        var reader = new LogReader(log);
        reader.openSync();
        var iterator = reader.iterator();
        iterator.skip(10, function (err) {
          assert.ifError(err);
          iterator.next(function (err, key, value) {
            assert.ifError(err);
            assert.equal('key10', key);
            assert.equal('value10', value);
            iterator.end();
            reader.close(done);
          });
        });
      });
    });

    describe('#isActive', function () {
      it('should return true if the iterator is active', function (done) {
        var reader = new LogReader(log);
        reader.openSync();
        var iterator = reader.iterator();
        iterator.next(function (err) {
          assert.ifError(err);
          assert(iterator.isActive());
          iterator.end();
          reader.close(done);
        });
      });

      it('should return false if the iterator is not active', function () {
        var reader = new LogReader(log);
        reader.openSync();
        var iterator = reader.iterator();
        assert(!iterator.isActive());
        iterator.end();
        reader.closeSync();
      });
    });
  });
});
