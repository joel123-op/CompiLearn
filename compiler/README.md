# Compiler Core

This directory will contain the C++ compiler pipeline.

Current:

- `lexer.l`: Flex lexer for Tiny C tokens
- `parser.y`: Bison parser for declarations, assignments, expressions, blocks, conditionals, loops, print, and return
- The API accepts a beginner C-style `int main()` wrapper and `#include` lines, and translates `printf(...)` to the compiler's `print(...)` statement. This is a small educational language, not a full C compiler.
- AST generation currently lives in the Flask compiler service and transforms parser JSON into a cleaner tree
- Semantic analysis and symbol table generation currently live in the Flask compiler service
- IR generation and optimization currently live in the Flask compiler service
- Assembly generation currently lives in the Flask compiler service
- VM execution currently lives in the Flask compiler service
- `Makefile`: builds `build/lexer`

Planned structure:

- `ast/`: AST node definitions and builders
- `semantic/`: semantic analyzer
- `symbol/`: symbol table
- `ir/`: three-address code generation
- `optimizer/`: optimization passes
- `codegen/`: assembly generation
- `vm/`: optional virtual machine

## Build

```bash
make all
```

## Direct Tests

```bash
printf 'int a = 5;\n' | ./build/lexer
printf 'int a = 5;\nprint(a);\n' | ./build/parser
```

Output shape:

```json
{
  "tokens": [
    {
      "lexeme": "int",
      "type": "Keyword",
      "line": 1,
      "column": 1
    }
  ],
  "success": true,
  "errors": []
}
```

Parser output shape:

```json
{
  "success": true,
  "status": "success",
  "errors": [],
  "parse_tree": {
    "name": "Program",
    "children": []
  }
}
```
