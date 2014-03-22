
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
  before(setup);
  after(teardown);

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
});

describe('hashSync', function () {
  before(setup);
  after(teardown);

  it('should convert an existing log to a hash', function () {
    sparkey.hashSync(log, hash);
    assert(fs.existsSync(hash));
    assert(fs.existsSync(log));
  });
});

