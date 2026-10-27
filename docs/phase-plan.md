# CompilerViz Phase Plan

## Phase 1: Foundation

- Create project structure
- Build React dashboard shell
- Build Flask API shell
- Connect frontend compile action to backend
- Return structured compiler-stage JSON

## Phase 2: Lexer

- Add Flex lexer
- Return token stream
- Render token table

Status: complete.

## Phase 3: Parser

- Add Bison parser
- Return parser status and syntax errors
- Prepare parse tree data

Status: complete.

## Phase 4: Trees

- Generate parse tree
- Generate AST
- Visualize both in frontend

Status: complete.

## Phase 5: Semantics

- Add declarations, scopes, and type checks
- Return semantic diagnostics
- Generate symbol table

Status: complete.

## Phase 6: IR and Optimization

- Generate three-address code
- Add constant folding
- Add simple dead code elimination
- Show before/after comparison

Status: complete.

## Phase 7: Code Generation

- Generate simple register-style assembly

## Phase 8: Polish

- Add dark mode refinements
- Add VM, AI explanations, export reports, and animations as stretch work
