
BINS = node_modules/.bin
SRC = $(wildcard src/*.*) $(wildcard src/*/*.*) $(deps/**/*.*)

build: $(SRC)
	@node-gyp rebuild

test: build
	@$(BINS)/mocha -R spec

clean:
	node-gyp clean
	rm -f *.spi *.spl

.PHONY: clean test
