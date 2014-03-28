
var sparkey = require('..');
var LogWriter = sparkey.LogWriter;
var fs = require('fs');
var join = require('path').join;
var assert = require('assert');

var log = join(__dirname, 'test.spl');
var hash = join(__dirname, 'test.spi');

function rm(file) {
  try {
    fs.unlinkSync(file);
  } catch (e) {}
}

function setup(done) {
  rm(log);
  rm(hash);
  var writer = new LogWriter(log);
  writer.open(function (err) {
    for (var i = 0; i < 100; i++) {
      writer.putSync('key_' + i, 'value_' + i);
    }
    writer.close(done);
  });
}

function teardown() {
  rm(log);
  rm(hash);
}

describe('hash', function () {
  beforeEach(setup);
  afterEach(teardown);

  it('should convert an existing log to a hash', function (done) {
    sparkey.hash(log, hash, function (err) {
      if (err) {
        return done(err);
      }

      assert(fs.existsSync(hash));
      assert(fs.existsSync(log));
      done();
    });
  });

  describe('given a hash size', function () {
    it('should error if the hash size is invalid', function (done) {
      sparkey.hash(log, hash, 1111, function (err) {
        assert(err);
        done();
      });
    });

    // see sparkey.h#L587-L590
    [0, 4, 8].forEach(function (hashSize) {
      it('should allow the hash size of ' + hashSize, function (done) {
        sparkey.hash(log, hash, hashSize, function (err) {
          assert.ifError(err);
          done();
        });
      });
    });
  });
});

describe('hashSync', function () {
  beforeEach(setup);
  afterEach(teardown);

  it('should convert an existing log to a hash', function () {
    sparkey.hashSync(log, hash);
    assert(fs.existsSync(hash));
    assert(fs.existsSync(log));
  });

  describe('given a hash size', function () {
    it('should error if the hash size is invalid', function () {
      var err;
      try {
        sparkey.hashSync(log, hash, 4567);
      } catch (e) {
        err = e;
      }
      assert(err);
    });

    // see sparkey.h#L587-L590
    [0, 4, 8].forEach(function (hashSize) {
      it('should allow the hash size of ' + hashSize, function () {
        sparkey.hashSync(log, hash, hashSize);
      });
    });
  });
});

