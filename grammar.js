/// <reference types="tree-sitter-cli/dsl" />

// Helper: comma-separated list with optional trailing comma
function commaSep1(rule) {
  return seq(rule, repeat(seq(",", rule)), optional(","));
}

function commaSep(rule) {
  return optional(commaSep1(rule));
}

module.exports = grammar({
  name: "usl",

  externals: ($) => [$._indent, $._dedent, $._newline],

  extras: ($) => [/[ \t\r]/, $.comment],

  conflicts: ($) => [[$.qualified_name]],

  rules: {
    source_file: ($) => repeat($._declaration),

    // -----------------------------------------------------------
    // Top-level declarations
    // -----------------------------------------------------------
    _declaration: ($) =>
      choice(
        $.package_declaration,
        $.extern_uses,
        $.import_declaration,
        $.module_declaration,
        $.entity_declaration,
        $.interface_declaration,
        $.enum_declaration,
        $.function_declaration,
        $.type_alias,
        $._newline,
      ),

    // -----------------------------------------------------------
    // package myapp
    // -----------------------------------------------------------
    package_declaration: ($) =>
      seq("package", field("name", $.identifier), $._newline),

    // -----------------------------------------------------------
    // extern uses auth-sdk
    // -----------------------------------------------------------
    extern_uses: ($) =>
      seq("extern", "uses", field("name", $.identifier), $._newline),

    // -----------------------------------------------------------
    // import auth.User
    // import auth.*
    // import billing.{ Invoice, Payment }
    // -----------------------------------------------------------
    import_declaration: ($) =>
      seq(
        "import",
        choice(
          $.wildcard_import,
          $.selective_import,
          $.qualified_name,
        ),
        $._newline,
      ),

    wildcard_import: ($) => seq($.qualified_name, ".", "*"),

    selective_import: ($) =>
      seq(
        $.qualified_name,
        ".",
        "{",
        commaSep1($.identifier),
        "}",
      ),

    // -----------------------------------------------------------
    // module <name>
    //   <declarations>
    // -----------------------------------------------------------
    module_declaration: ($) =>
      seq(
        "module",
        field("name", $.identifier),
        $._newline,
        $._indent,
        repeat($._declaration),
        $._dedent,
      ),

    // -----------------------------------------------------------
    // entity <Name> [<TypeParams>] [implements I1, I2]
    //   <members>
    // -----------------------------------------------------------
    entity_declaration: ($) =>
      seq(
        "entity",
        field("name", $.identifier),
        optional($.type_parameters),
        optional($.implements_clause),
        choice(
          seq($._newline, $._indent, repeat1($._member), $._dedent),
          $._newline,
        ),
      ),

    // -----------------------------------------------------------
    // interface <Name> [<TypeParams>] [implements I1, I2]
    //   <methods>
    // -----------------------------------------------------------
    interface_declaration: ($) =>
      seq(
        "interface",
        field("name", $.identifier),
        optional($.type_parameters),
        optional($.implements_clause),
        choice(
          seq($._newline, $._indent, repeat1($._method), $._dedent),
          $._newline,
        ),
      ),

    // -----------------------------------------------------------
    // enum <Name> [<TypeParams>]
    //   Variant1
    //   Variant2(data: Type)
    // -----------------------------------------------------------
    enum_declaration: ($) =>
      seq(
        "enum",
        field("name", $.identifier),
        optional($.type_parameters),
        $._newline,
        $._indent,
        repeat1($.enum_variant),
        $._dedent,
      ),

    enum_variant: ($) =>
      seq(
        field("name", $.identifier),
        optional(seq("(", commaSep1($._variant_field), ")")),
        $._newline,
      ),

    _variant_field: ($) =>
      choice(
        $.field,   // named: name: Type
        $._type,   // positional: just a type
      ),

    // -----------------------------------------------------------
    // fn <name> [<TypeParams>] (<params>) [-> ReturnType]
    // -----------------------------------------------------------
    function_declaration: ($) =>
      seq(
        "fn",
        field("name", $.identifier),
        optional($.type_parameters),
        "(", commaSep($.parameter), ")",
        optional(seq("->", $._type)),
        $._newline,
      ),

    // -----------------------------------------------------------
    // type Name = TargetType
    // -----------------------------------------------------------
    type_alias: ($) =>
      seq("type", field("name", $.identifier), "=", field("value", $._type), $._newline),

    // -----------------------------------------------------------
    // Shared constructs
    // -----------------------------------------------------------

    implements_clause: ($) =>
      seq("implements", commaSep1($._type)),

    type_parameters: ($) =>
      seq("<", commaSep1($.identifier), ">"),

    _member: ($) =>
      choice(
        $.field_declaration,
        $.function_declaration,
      ),

    _method: ($) => $.function_declaration,

    field_declaration: ($) =>
      seq(field("name", $.identifier), ":", field("type", $._type), $._newline),

    field: ($) => seq(field("name", $.identifier), ":", field("type", $._type)),

    parameter: ($) =>
      choice(
        seq(field("name", $.identifier), ":", field("type", $._type)),  // named: creds: Credentials
        field("type", $._type),                                         // positional: Credentials
      ),

    // -----------------------------------------------------------
    // Type expressions
    // -----------------------------------------------------------
    _type: ($) =>
      choice(
        $.optional_type,
        $.generic_type,
        $.function_type,
        $.tuple_type,
        $.qualified_name,
      ),

    optional_type: ($) => prec(2, seq($._type, "?")),

    generic_type: ($) =>
      prec(1, seq($.qualified_name, "<", commaSep1($._type), ">")),

    function_type: ($) =>
      seq("(", commaSep($._type), ")", "->", $._type),

    tuple_type: ($) =>
      seq("(", commaSep1($._type), ")"),

    // -----------------------------------------------------------
    // Names
    // -----------------------------------------------------------
    qualified_name: ($) =>
      seq($.identifier, repeat(seq(".", $.identifier))),

    identifier: (_) => /[a-zA-Z_][a-zA-Z0-9_-]*/,

    // -----------------------------------------------------------
    // Comments
    // -----------------------------------------------------------
    comment: (_) =>
      token(
        choice(
          seq("//", /.*/),
          seq("///", /.*/),
        ),
      ),
  },
});
