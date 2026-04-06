# CLAUDE.md

## Build & Test

```sh
npm install              # install dependencies (includes tree-sitter-cli)
npx tree-sitter generate # regenerate parser from grammar.js
npx tree-sitter test     # run all tests
npx tree-sitter parse <file.usl>  # parse a file
make                     # build C shared/static libraries
make clean               # remove build artifacts
```

## Project Structure

- `grammar.js` - Grammar definition (the source of truth)
- `src/scanner.c` - External scanner handling indent/dedent/newline tokens
- `src/parser.c` - Generated file, do not edit manually
- `test/corpus/*.txt` - Tree-sitter test cases (each file tests a language construct)
- `bindings/` - Node, Rust, Python, Go, Swift bindings

## Key Concepts

- USL uses **indentation-based scoping**. The external scanner (`src/scanner.c`) emits `_indent`, `_dedent`, and `_newline` tokens.
- After changing `grammar.js`, always run `npx tree-sitter generate` before testing.
- `src/parser.c` and `src/grammar.json` are generated artifacts — never edit them directly.
