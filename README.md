# CompilerViz

CompilerViz is an interactive compiler construction and visualization platform.

Phase 1 includes:

- React + TypeScript + Vite frontend
- Flask backend
- Initial `/compile` REST API
- Compiler dashboard shell
- Folder structure for the compiler pipeline

Phase 2 adds:

- Flex-based lexer
- Real token stream generation
- `/lexer` REST API
- Token metrics from lexer output

Phase 3 adds:

- Bison-based parser
- `/parser` REST API
- Syntax error reporting with line and column
- Parse tree JSON returned to the frontend

Phase 4 adds:

- AST generation from the parse tree
- `/ast` REST API
- Cleaner compiler-style trees for declarations, assignments, expressions, blocks, loops, print, and return

Phase 5 adds:

- Semantic analysis
- Scoped symbol table generation
- Duplicate declaration checks
- Undeclared variable checks
- Basic type compatibility checks
- `/semantic` and `/symbol-table` REST APIs

Phase 6 adds:

- Three-address intermediate code generation
- Constant folding and algebraic simplification
- Copy propagation
- Simple dead-store elimination
- `/ir` and `/optimize` REST APIs

Phase 7 adds:

- Educational pseudo-assembly generation
- Symbol and temporary allocation
- Arithmetic, comparison, branch, print, and return instructions
- `/assembly` REST API

Phase 8 adds:

- Virtual machine execution for generated assembly
- Register, memory, stack, output, and trace snapshots
- `/vm` REST API

Phase 9 adds:

- Animated compiler pipeline stage visualizer
- Step-by-step VM execution controls
- Per-instruction register, memory, and output snapshots

Phase 10 adds:

- AI-style compiler error explanations
- Complete printable compiler report export
- Interactive symbol table, parse tree, and AST explorers

## Run Backend

```bash
cd backend
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python app.py
```

Backend URL: `http://localhost:5003`

## Run Frontend

```bash
cd frontend
npm install
npm run dev
```

Frontend URL: `http://localhost:5175`

## Build Compiler Core

```bash
cd compiler
make all
```

You can test the lexer and parser directly:

```bash
printf 'int a = 5;\n' | ./build/lexer
printf 'int a = 5;\nprint(a);\n' | ./build/parser
```

## Current API

```http
POST /compile
POST /lexer
POST /parser
POST /ast
POST /semantic
POST /symbol-table
POST /ir
POST /optimize
POST /assembly
POST /vm
POST /explain
```

Body:

```json
{
  "source": "int a = 5;"
}
```

The response currently contains real lexer, parser, parse tree, AST, semantic, symbol table, IR, optimized IR, assembly, VM execution output, and AI-style error help.
