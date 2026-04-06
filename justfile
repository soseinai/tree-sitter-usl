# tree-sitter-usl

# Generate parser from grammar.js
generate:
    npx tree-sitter generate 
# Run all tests
test: generate
    npx tree-sitter test

# Parse a file
parse file:
    npx tree-sitter parse {{file}}

# Highlight a file
highlight file:
    npx tree-sitter highlight {{file}}

# Build C library (static + shared) into dist/
build: generate
    mkdir -p build dist
    cc -Isrc -std=c11 -fPIC -c src/parser.c -o build/parser.o
    cc -Isrc -std=c11 -fPIC -c src/scanner.c -o build/scanner.o
    ar rcs dist/libtree-sitter-usl.a build/parser.o build/scanner.o
    cc -dynamiclib -Wl,-install_name,libtree-sitter-usl.dylib build/parser.o build/scanner.o -o dist/libtree-sitter-usl.dylib

# Build all language bindings
bindings: generate binding-node binding-rust binding-python binding-go binding-swift

# Build Node.js binding
binding-node:
    cd bindings/node && npm install && npx node-gyp rebuild

# Build Rust binding
binding-rust:
    cd bindings/rust && cargo build

# Build Python binding
binding-python:
    cd bindings/python && pip install -e .

# Build Go binding
binding-go:
    cd bindings/go && go build

# Build Swift binding
binding-swift:
    cd bindings/swift && swift build

# Remove build artifacts
clean:
    rm -rf build dist
    rm -rf bindings/rust/target
    rm -rf bindings/node/build
    rm -rf bindings/swift/.build
