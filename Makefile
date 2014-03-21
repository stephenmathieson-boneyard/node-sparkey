
BINS = node_modules/.bin
SRC = $(wildcard src/*/*.*) $(deps/**/*.*)

build: node_modules $(SRC)
	@node-gyp rebuild

test: build
	@$(BINS)/mocha -R spec

node_modules: package.json
	@npm install

clean:
	node-gyp clean
	rm -f *.spi *.spl

.PHONY: clean test
