
var sparkey = require('..');
var LogReader = sparkey.LogReader;
var LogWriter = sparkey.LogWriter;
var HashReader = sparkey.HashReader;
var path = require('path');
var assert = require('assert');
var rm = require('fs').unlinkSync;


var log = path.join(__dirname, 'log.spl');
var hash = path.join(__dirname, 'log.spi');

describe('HashReader', function () {
  beforeEach(function (done) {
    try {
      rm(log);
      rm(hash);
    } catch (e) {}

    var writer = new LogWriter(log);
    writer.open(function (err) {
      if (err) return done(err);
      var i = 0;
      writer.put('key_' + i, 'value_' + i, write);
      function write(err) {
        if (err) return done(err);
        if (100 == ++i) {
          writer.close(function (err) {
            if (err) return done(err);
            sparkey.hash(log, hash, done);
          });
        } else {
          writer.put('key_' + i, 'value_' + i, write);
        }
      }
    });
  });

  describe('#open', function () {
    it('should open a existing hash/log', function (done) {
      var reader = new HashReader(hash, log);
      reader.open(function (err) {
        assert.ifError(err);
        reader.close(done);
      });
    });

    it('should error if the hash is missing', function (done) {
      var reader = new HashReader('nope', log);
      reader.open(function (err) {
        assert(err);
        done();
      });
    });

    it('should error if the log is missing', function (done) {
      var reader = new HashReader(hash, 'nope');
      reader.open(function (err) {
        assert(err);
        done();
      });
    });

    it('should error if the hash is a log', function (done) {
      var reader = new HashReader(log, log);
      reader.open(function (err) {
        assert(err);
        done();
      });
    });

    it('should error if the hash doesn\'t reference the log', function (done) {
      var writer = new LogWriter(path.join(__dirname, 'log2.spl'));
      writer.openSync();
      for (var i = 0; i < 100; i++) {
        writer.putSync('key_' + i, 'value_' + i);
      }
      writer.closeSync();

      var reader = new HashReader(hash, path.join(__dirname, 'log2.spl'));
      reader.open(function (err) {
        assert(err);
        done();
      });
    });
  });

  describe('#openSync', function () {
    it('should open an existing hash/log', function () {
      var reader = new HashReader(hash, log);
      reader.openSync();
      reader.iterator().end();
      reader.closeSync();
    });

    it('should throw if the hash or log are missing', function () {
      var err;
      var reader = new HashReader(hash, log + 'sdfsdf');
      try {
        reader.openSync();
      } catch (e) {
        err = e;
      }
      assert(err);
    });
  });

  describe('#close', function () {
    it('should close the hash reader', function (done) {
      var reader = new HashReader(hash, log);
      reader.open(function (err) {
        assert.ifError(err);
        reader.close(done);
      });
    });
  });

  describe('#closeSync', function () {
    it('should close the hash reader', function (done) {
      var reader = new HashReader(hash, log);
      reader.open(function (err) {
        assert.ifError(err);
        reader.closeSync();
        done();
      });
    });
  });

  describe('#count', function () {
    it('should return the number if entries in the hash', function (done) {
      var reader = new HashReader(hash, log);
      reader.open(function (err) {
        assert.ifError(err);
        assert.equal(100, reader.count());
        reader.close(done);
      });
    });
  });

  describe('#iterator', function () {
    it('should create a HashIterator', function (done) {
      var reader = new HashReader(hash, log);
      reader.open(function (err) {
        assert.ifError(err);
        var iterator = reader.iterator();
        assert(iterator);
        iterator.end();
        reader.close(done);
      });
    });

    describe('#next', function () {
      it('should get the next key/value/type', function (done) {
        var reader = new HashReader(hash, log);
        reader.open(function (err) {
          assert.ifError(err);
          var iterator = reader.iterator();
          var i = -1;
          iterator.next(callback);
          function callback(err, key, value, type) {
            assert.ifError(err);
            if (100 == ++i) {
              assert.equal(null, key);
              assert.equal(null, value);
              assert.equal(null, type);
              iterator.end();
              reader.close(done);
            } else {
              assert.equal('key_' + i, key);
              assert.equal('value_' + i, value);
              assert.equal(sparkey.SPARKEY_ENTRY_PUT, type);
              iterator.next(callback);
            }
          }
        });
      });

      describe('#skip', function () {
        it('should skip over a number of entries', function (done) {
          var reader = new HashReader(hash, log);
          reader.openSync();
          var iterator = reader.iterator();
          iterator.skip(55, function (err) {
            assert.ifError(err);
            iterator.next(function (err, key, value) {
              assert.ifError(err);
              assert.equal('key_55', key);
              assert.equal('value_55', value);
              iterator.end();
              reader.close(done);
            });
          });
        });

        it('should allow users to skip past the end', function (done) {
          var reader = new HashReader(hash, log);
          reader.openSync();
          var iterator = reader.iterator();
          iterator.skip(101, function (err) {
            assert.ifError(err);
            iterator.next(function (err, key, value) {
              assert.ifError(err);
              assert.equal(null, key);
              assert.equal(null, value);
              iterator.end();
              reader.close(done);
            });
          });
        });
      });
    });

    describe('#get', function () {
      it('should get a key\'s value', function (done) {
        var reader = new HashReader(hash, log);
        reader.open(function (err) {
          assert.ifError(err);
          var iterator = reader.iterator();
          iterator.get('key_15', function (err, value) {
            assert.ifError(err);
            assert.equal('value_15', value);
            iterator.end();
            reader.close(done);
          });
        });
      });

      it('should provide NULL if the key is not found', function (done) {
        var reader = new HashReader(hash, log);
        reader.open(function (err) {
          assert.ifError(err);
          var iterator = reader.iterator();
          iterator.get('a key that doesnt exist', function (err, value) {
            assert.ifError(err);
            assert.equal(null, value);
            iterator.end();
            reader.close(done);
          });
        });
      });
    });
  });
});
