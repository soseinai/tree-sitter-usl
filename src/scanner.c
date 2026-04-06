#include "tree_sitter/parser.h"
#include <assert.h>
#include <string.h>

// Token types matching the externals array in grammar.js
enum TokenType {
  INDENT,
  DEDENT,
  NEWLINE,
};

// Maximum nesting depth
#define MAX_DEPTH 64
#define NO_QUEUED_INDENT 0xFFFF

typedef struct {
  uint8_t depth;
  uint16_t indent_stack[MAX_DEPTH];
  uint16_t queued_indent;  // pending indent level after NEWLINE, or NO_QUEUED_INDENT
} Scanner;

void *tree_sitter_usl_external_scanner_create(void) {
  Scanner *scanner = calloc(1, sizeof(Scanner));
  scanner->depth = 0;
  scanner->indent_stack[0] = 0;
  scanner->queued_indent = NO_QUEUED_INDENT;
  return scanner;
}

void tree_sitter_usl_external_scanner_destroy(void *payload) {
  free(payload);
}

unsigned tree_sitter_usl_external_scanner_serialize(void *payload,
                                                     char *buffer) {
  Scanner *scanner = (Scanner *)payload;
  unsigned size = 0;

  buffer[size++] = scanner->depth;
  for (uint8_t i = 0; i <= scanner->depth; i++) {
    buffer[size++] = (char)(scanner->indent_stack[i] & 0xFF);
    buffer[size++] = (char)((scanner->indent_stack[i] >> 8) & 0xFF);
  }

  buffer[size++] = (char)(scanner->queued_indent & 0xFF);
  buffer[size++] = (char)((scanner->queued_indent >> 8) & 0xFF);

  return size;
}

void tree_sitter_usl_external_scanner_deserialize(void *payload,
                                                    const char *buffer,
                                                    unsigned length) {
  Scanner *scanner = (Scanner *)payload;

  if (length == 0) {
    scanner->depth = 0;
    scanner->indent_stack[0] = 0;
    scanner->queued_indent = NO_QUEUED_INDENT;
    return;
  }

  unsigned pos = 0;
  scanner->depth = (uint8_t)buffer[pos++];
  for (uint8_t i = 0; i <= scanner->depth; i++) {
    uint8_t lo = (uint8_t)buffer[pos++];
    uint8_t hi = (uint8_t)buffer[pos++];
    scanner->indent_stack[i] = (uint16_t)(lo | (hi << 8));
  }

  if (pos + 1 < length) {
    uint8_t lo = (uint8_t)buffer[pos++];
    uint8_t hi = (uint8_t)buffer[pos++];
    scanner->queued_indent = (uint16_t)(lo | (hi << 8));
  } else {
    scanner->queued_indent = NO_QUEUED_INDENT;
  }
}

static void skip(TSLexer *lexer) { lexer->advance(lexer, true); }

bool tree_sitter_usl_external_scanner_scan(void *payload, TSLexer *lexer,
                                            const bool *valid_symbols) {
  Scanner *scanner = (Scanner *)payload;

  // At end of file, emit remaining DEDENTs then stop
  if (lexer->eof(lexer)) {
    if (valid_symbols[DEDENT] && scanner->depth > 0) {
      scanner->depth--;
      lexer->result_symbol = DEDENT;
      return true;
    }
    return false;
  }

  // Handle pending indent/dedent queued from a previous NEWLINE emission.
  // After a newline with changed indentation, we first emit NEWLINE, then
  // on the next call we emit the INDENT or DEDENT(s) here.
  if (scanner->queued_indent != NO_QUEUED_INDENT) {
    uint16_t current = scanner->indent_stack[scanner->depth];
    uint16_t target = scanner->queued_indent;

    if (target > current && valid_symbols[INDENT]) {
      if (scanner->depth < MAX_DEPTH - 1) {
        scanner->depth++;
        scanner->indent_stack[scanner->depth] = target;
      }
      scanner->queued_indent = NO_QUEUED_INDENT;
      lexer->result_symbol = INDENT;
      return true;
    }

    if (target < current && valid_symbols[DEDENT] && scanner->depth > 0) {
      scanner->depth--;
      // Clear queue once we've dedented to the target level
      if (scanner->indent_stack[scanner->depth] <= target) {
        scanner->queued_indent = NO_QUEUED_INDENT;
      }
      lexer->result_symbol = DEDENT;
      return true;
    }

    // Target matches current, or we can't emit what's needed — clear queue
    scanner->queued_indent = NO_QUEUED_INDENT;
    return false;
  }

  // Look for newlines
  bool found_newline = false;
  while (lexer->lookahead == '\n' || lexer->lookahead == '\r') {
    found_newline = true;
    skip(lexer);
  }

  if (!found_newline) {
    return false;
  }

  // Count indentation of the next line (spaces and tabs)
  uint16_t indent = 0;
  while (lexer->lookahead == ' ' || lexer->lookahead == '\t') {
    if (lexer->lookahead == '\t') {
      indent += 8 - (indent % 8); // tab stops at multiples of 8
    } else {
      indent++;
    }
    skip(lexer);
  }

  // Skip blank lines (lines that are only whitespace)
  if (lexer->lookahead == '\n' || lexer->lookahead == '\r') {
    if (valid_symbols[NEWLINE]) {
      lexer->result_symbol = NEWLINE;
      return true;
    }
    return false;
  }

  // Skip comment-only lines for indentation purposes
  if (lexer->lookahead == '/' && valid_symbols[NEWLINE]) {
    lexer->result_symbol = NEWLINE;
    return true;
  }

  uint16_t current_indent = scanner->indent_stack[scanner->depth];

  if (indent == current_indent) {
    // Same level — just emit NEWLINE
    if (valid_symbols[NEWLINE]) {
      lexer->result_symbol = NEWLINE;
      return true;
    }
    return false;
  }

  // Indent changed — emit NEWLINE first, queue the indent change for the next call
  if (valid_symbols[NEWLINE]) {
    scanner->queued_indent = indent;
    lexer->result_symbol = NEWLINE;
    return true;
  }

  // NEWLINE not valid — try to emit INDENT/DEDENT directly
  if (indent > current_indent && valid_symbols[INDENT]) {
    if (scanner->depth < MAX_DEPTH - 1) {
      scanner->depth++;
      scanner->indent_stack[scanner->depth] = indent;
    }
    lexer->result_symbol = INDENT;
    return true;
  }

  if (indent < current_indent && valid_symbols[DEDENT] && scanner->depth > 0) {
    scanner->queued_indent = indent;
    scanner->depth--;
    lexer->result_symbol = DEDENT;
    return true;
  }

  return false;
}
