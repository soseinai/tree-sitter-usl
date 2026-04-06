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
                    "Package.swift",
                ],
                sources: [
                    "src/parser.c",
                    "src/scanner.c",
                ],
                resources: [
                    .copy("queries")
                ],
                publicHeadersPath: "TreeSitterUsl",
                cSettings: [.headerSearchPath("src")])
    ],
    cLanguageStandard: .c11
)
