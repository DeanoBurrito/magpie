# Magpie Toolchain

Magpie is my attempt at writing a compiler toolkit, and implementing a C-language frontend with it. Initially I plan to emit LLVM-IR and use their backend, but will replace that with my own backend at some point. It's inspired by the modular approach of LLVM, and as such magpie is mostly constructed of modules and a few tools/utility programs that provide the user-facing portions.

The project is also entirely isolated from the standard library, via the 'host API'. Any non-magpie functions must be called through this, which should make magpie easy to port to other platforms. Host API implementations can be found in `hosts/`.

Magpie isn't intended to be your next compiler and I dont ever intend for it to be. It's a personal project for my own fun and learning. Feel free to clone it and hack on it yourself, minor (correction and bug fix) PRs are welcome but I want to implement the major features myself.

core todo: plugins
cc todo: preprocessor + ingest phases, lexer (queue of preprocessed tokens, from peek), parser

## Status & Roadmap
- Host APIs:
    - [x] Linux.
- Modules:
    - Core:
        - [x] Allocator.
        - [x] Config.
        - [x] Diagnostics.
        - [ ] Plugin support.
        - [x] Source management + metadata.
        - [x] Primitive types.
    - C Compiler (CC)
        - [x] Preprocessor.
        - [x] Lexer.
        - [ ] AST.
        - [ ] Parser.
    - IR
    - Binman:
        - [ ] Infrastructure.
        - [ ] Elf backend.
        - [ ] PE backend.
    - Emit:
        - [ ] Infrastructure.
        - [ ] x86_64.
        - [ ] riscv64.
- Tools:
    - Magpiler (generic compiler driver).
        - [ ] C compiler driver.
    - Readbin (similar to readelf)
        - [ ] `readelf` compat mode.
