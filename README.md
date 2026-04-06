# tree-sitter-usl

A [tree-sitter](https://tree-sitter.github.io/tree-sitter/) grammar for USL (Universal Structure Language).

## Overview

USL is a structural modeling language with support for packages, modules, entities, interfaces, enums, functions, and type aliases. It uses indentation-based scoping.

```
package myapp

import auth.User
import billing.{ Invoice, Payment }

entity Order
  id: Id
  user: User
  items: List<Item>
  fn total() -> Money
```

## Development

### Prerequisites

- [Node.js](https://nodejs.org/)
- A C compiler (for building the shared library)

### Setup

```sh
npm install
```

### Generate the parser

After editing `grammar.js`, regenerate the parser:

```sh
just generate
```

### Run tests

```sh
just test
```

Test cases are in `test/corpus/`.

### Parse a file

```sh
just parse example.usl
```

### Build the C library

```sh
just build
```

### Build all language bindings

```sh
just bindings
```

### Clean build artifacts

```sh
just clean
```

## Project Structure

- `grammar.js` - Grammar definition
- `src/scanner.c` - External scanner for indentation handling
- `src/parser.c` - Generated parser (do not edit)
- `test/corpus/` - Test cases
- `bindings/` - Language bindings (Node.js, Rust, Python, Go, Swift)

## License

MIT
