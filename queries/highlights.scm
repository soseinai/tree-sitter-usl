; Keywords
[
  "package"
  "import"
  "module"
  "entity"
  "interface"
  "enum"
  "fn"
  "type"
  "extern"
  "uses"
  "implements"
] @keyword

; Operators and punctuation
"=" @operator
"->" @operator
"?" @operator
"." @punctuation.delimiter
"," @punctuation.delimiter
":" @punctuation.delimiter
"(" @punctuation.bracket
")" @punctuation.bracket
"<" @punctuation.bracket
">" @punctuation.bracket
"{" @punctuation.bracket
"}" @punctuation.bracket
"*" @operator

; Type declarations
(entity_declaration name: (identifier) @type.definition)
(interface_declaration name: (identifier) @type.definition)
(enum_declaration name: (identifier) @type.definition)
(type_alias name: (identifier) @type.definition)
(enum_variant name: (identifier) @type.definition)

; Function declarations
(function_declaration name: (identifier) @function)

; Module and package names
(module_declaration name: (identifier) @module)
(package_declaration name: (identifier) @module)
(extern_uses name: (identifier) @module)

; Type references
(type_parameters (identifier) @type)
(implements_clause (qualified_name) @type)
(generic_type (qualified_name) @type)
(field_declaration type: (_) @type)
(field type: (_) @type)

; Parameters
(parameter name: (identifier) @variable.parameter)

; Field declarations
(field_declaration name: (identifier) @property)
(field name: (identifier) @property)

; Comments
(comment) @comment
