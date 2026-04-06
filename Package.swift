// swift-tools-version:5.3
import PackageDescription

let package = Package(
    name: "TreeSitterUsl",
    products: [
        .library(name: "TreeSitterUsl", targets: ["TreeSitterUsl"]),
    ],
    dependencies: [],
    targets: [
        .target(name: "TreeSitterUsl",
                path: ".",
                exclude: [
                    "Cargo.toml",
                    "justfile",
                    "binding.gyp",
                    "bindings/c",
                    "bindings/go",
                    "bindings/node",
                    "bindings/python",
                    "bindings/rust",
                    "prebuilds",
                    "grammar.js",
                    "package.json",
                    "package-lock.json",
                    "pyproject.toml",
                    "setup.py",
                    "test",
                    "examples",
                    ".editorconfig",
                    ".github",
                    ".gitignore",
                    ".gitattributes",
                    ".gitmodules",
                ],
                sources: [
                    "src/parser.c",
                    "src/scanner.c",
                ],
                resources: [
                    .copy("queries")
                ],
                publicHeadersPath: "bindings/swift",
                cSettings: [.headerSearchPath("src")])
    ],
    cLanguageStandard: .c11
)
