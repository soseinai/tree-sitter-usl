# tree-sitter-usl

A [tree-sitter](https://tree-sitter.github.io/tree-sitter/) grammar for USL (Universal Structure Language).

## Overview

USL is a structural modeling language with support for packages, modules, entities, interfaces, enums, functions, and type aliases. Its intended use is for agents or tools that need to understand the overarching structure and shape of coding projects, without getting lost in the weeds of implementation. It is designed to be general enough to capture the shapes and relationships between entities across most modern languages.

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

## Using from the JVM

Tree-sitter grammars don't ship JVM bindings directly. Instead, use one of these JVM binding libraries to consume the grammar:

- **[KTreeSitter](https://github.com/tree-sitter/kotlin-tree-sitter)** (Kotlin, recommended) — Use the Gradle plugin to point at this repo's `src/parser.c` and `src/scanner.c`. Works on any JVM and Android.
- **[JTreeSitter](https://github.com/tree-sitter/java-tree-sitter)** (Java) — Load the compiled shared library (`just build` produces it in `dist/`) at runtime via Panama FFI. Requires JDK 22+.
- **[java-tree-sitter](https://github.com/seart-group/java-tree-sitter)** (Java) — Bundles many grammars into one library. Works on Java 11+ via JNI.

## Project Structure

- `grammar.js` - Grammar definition
- `src/scanner.c` - External scanner for indentation handling
- `src/parser.c` - Generated parser (do not edit)
- `test/corpus/` - Test cases
- `bindings/` - Language bindings (Node.js, Rust, Python, Go, Swift)

## License

MIT
