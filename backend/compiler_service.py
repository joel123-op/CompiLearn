from __future__ import annotations

from dataclasses import dataclass, asdict
import json
from pathlib import Path
import re
import subprocess
from typing import Any


@dataclass
class CompilationResult:
    success: bool
    message: str
    tokens: list[dict[str, Any]]
    parser: dict[str, Any]
    parse_tree: dict[str, Any]
    ast: dict[str, Any]
    semantic: dict[str, Any]
    symbol_table: list[dict[str, Any]]
    ir: list[str]
    optimized_ir: list[str]
    optimizations: list[str]
    assembly: list[str]
    vm: dict[str, Any]
    ai_explanation: dict[str, Any]
    metrics: dict[str, int]

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


PROJECT_ROOT = Path(__file__).resolve().parents[1]
COMPILER_DIR = PROJECT_ROOT / "compiler"
LEXER_BIN = COMPILER_DIR / "build" / "lexer"
PARSER_BIN = COMPILER_DIR / "build" / "parser"


def normalize_c_source(source: str) -> str:
    def preserve_lines(text: str) -> str:
        return "".join("\n" if character == "\n" else " " for character in text)

    source = re.sub(
        r'(?m)^[ \t]*#[ \t]*include[ \t]*[<"][^>\"]+[>\"][ \t]*',
        lambda match: preserve_lines(match.group()),
        source,
    )

    main_match = re.match(
        r"\A\s*int\s+main\s*\(\s*(?:void)?\s*\)\s*\{",
        source,
    )
    if main_match and source.rstrip().endswith("}"):
        source = preserve_lines(main_match.group()) + source[main_match.end() :]
        closing_brace = source.rfind("}")
        source = source[:closing_brace] + " " + source[closing_brace + 1 :]

    return re.sub(r"\bprintf(?=\s*\()", "print ", source)


def empty_metrics() -> dict[str, int]:
    return {
        "tokens": 0,
        "identifiers": 0,
        "keywords": 0,
        "operators": 0,
        "loops": 0,
        "functions": 0,
    }


def calculate_metrics(tokens: list[dict[str, Any]]) -> dict[str, int]:
    keywords = [token["lexeme"] for token in tokens if token.get("type") == "Keyword"]

    return {
        "tokens": len(tokens),
        "identifiers": sum(1 for token in tokens if token.get("type") == "Identifier"),
        "keywords": len(keywords),
        "operators": sum(1 for token in tokens if token.get("type") == "Operator"),
        "loops": sum(1 for keyword in keywords if keyword in {"for", "while"}),
        "functions": sum(1 for token in tokens if token.get("lexeme") == "("),
    }


def ensure_lexer_built() -> None:
    if LEXER_BIN.exists():
        return

    subprocess.run(
        ["make", "lexer"],
        cwd=COMPILER_DIR,
        check=True,
        capture_output=True,
        text=True,
    )


def ensure_parser_built() -> None:
    if PARSER_BIN.exists():
        return

    subprocess.run(
        ["make", "parser"],
        cwd=COMPILER_DIR,
        check=True,
        capture_output=True,
        text=True,
    )


def lex_source(source: str) -> dict[str, Any]:
    normalized_source = source.strip()

    if not normalized_source:
        return {
            "success": False,
            "message": "No source code provided.",
            "tokens": [],
            "errors": [{"message": "Source code is empty.", "line": 1, "column": 1}],
            "metrics": empty_metrics(),
        }

    try:
        ensure_lexer_built()
        completed = subprocess.run(
            [str(LEXER_BIN)],
            input=normalize_c_source(source),
            capture_output=True,
            text=True,
            check=False,
        )
    except (OSError, subprocess.CalledProcessError) as exc:
        return {
            "success": False,
            "message": f"Lexer could not run: {exc}",
            "tokens": [],
            "errors": [{"message": str(exc), "line": 1, "column": 1}],
            "metrics": empty_metrics(),
        }

    try:
        lexer_result = json.loads(completed.stdout)
    except json.JSONDecodeError:
        return {
            "success": False,
            "message": "Lexer returned invalid JSON.",
            "tokens": [],
            "errors": [
                {
                    "message": completed.stderr or completed.stdout or "Unknown lexer error.",
                    "line": 1,
                    "column": 1,
                }
            ],
            "metrics": empty_metrics(),
        }

    tokens = lexer_result.get("tokens", [])

    return {
        "success": bool(lexer_result.get("success")),
        "message": "Lexical analysis completed." if lexer_result.get("success") else "Lexical analysis found errors.",
        "tokens": tokens,
        "errors": lexer_result.get("errors", []),
        "metrics": calculate_metrics(tokens),
    }


def parse_source(source: str) -> dict[str, Any]:
    lexer_result = lex_source(source)

    if not lexer_result["success"]:
        return {
            "success": False,
            "status": "skipped",
            "message": "Parsing skipped because lexical analysis failed.",
            "errors": lexer_result["errors"],
            "parse_tree": {"name": "Program", "children": []},
            "tokens": lexer_result["tokens"],
            "metrics": lexer_result["metrics"],
        }

    try:
        ensure_parser_built()
        completed = subprocess.run(
            [str(PARSER_BIN)],
            input=normalize_c_source(source),
            capture_output=True,
            text=True,
            check=False,
        )
    except (OSError, subprocess.CalledProcessError) as exc:
        return {
            "success": False,
            "status": "error",
            "message": f"Parser could not run: {exc}",
            "errors": [{"message": str(exc), "line": 1, "column": 1}],
            "parse_tree": {"name": "Program", "children": []},
            "tokens": lexer_result["tokens"],
            "metrics": lexer_result["metrics"],
        }

    try:
        parser_result = json.loads(completed.stdout)
    except json.JSONDecodeError:
        return {
            "success": False,
            "status": "error",
            "message": "Parser returned invalid JSON.",
            "errors": [
                {
                    "message": completed.stderr or completed.stdout or "Unknown parser error.",
                    "line": 1,
                    "column": 1,
                }
            ],
            "parse_tree": {"name": "Program", "children": []},
            "tokens": lexer_result["tokens"],
            "metrics": lexer_result["metrics"],
        }

    return {
        "success": bool(parser_result.get("success")),
        "status": parser_result.get("status", "error"),
        "message": "Parsing successful." if parser_result.get("success") else "Syntax error found.",
        "errors": parser_result.get("errors", []),
        "parse_tree": parser_result.get("parse_tree", {"name": "Program", "children": []}),
        "tokens": lexer_result["tokens"],
        "metrics": lexer_result["metrics"],
    }


def ast_leaf(name: str) -> dict[str, Any]:
    return {"name": name, "children": []}


def split_label(name: str, prefix: str) -> str:
    if name.startswith(prefix):
        return name.removeprefix(prefix).strip()
    return name


def transform_expression(node: dict[str, Any]) -> dict[str, Any]:
    name = node.get("name", "")
    children = node.get("children", [])

    if name.startswith("Operator: "):
        operator = split_label(name, "Operator:")
        return {
            "name": operator,
            "children": [transform_expression(child) for child in children],
        }

    if name.startswith("Identifier: "):
        return ast_leaf(split_label(name, "Identifier:"))

    if name.startswith("Integer: "):
        return ast_leaf(split_label(name, "Integer:"))

    if name.startswith("Float: "):
        return ast_leaf(split_label(name, "Float:"))

    if name.startswith("String: "):
        return ast_leaf(split_label(name, "String:"))

    if name.startswith("Character: "):
        return ast_leaf(split_label(name, "Character:"))

    return {
        "name": name,
        "children": [transform_expression(child) for child in children],
    }


def transform_statement(node: dict[str, Any]) -> dict[str, Any] | None:
    name = node.get("name", "")
    children = node.get("children", [])

    if name in {"Program", "Statements"}:
        statements: list[dict[str, Any]] = []
        for child in children:
            transformed = transform_statement(child)
            if transformed is None:
                continue
            if transformed["name"] == "Program":
                statements.extend(transformed["children"])
            else:
                statements.append(transformed)
        return {"name": "Program", "children": statements}

    if name == "Declaration":
        type_name = "unknown"
        identifier = "unknown"
        init_node = None
        found_identifier = False

        for child in children:
            child_name = child.get("name", "")
            if child_name.startswith("Type: "):
                type_name = split_label(child_name, "Type:")
            elif child_name.startswith("Identifier: ") and not found_identifier:
                identifier = split_label(child_name, "Identifier:")
                found_identifier = True
            else:
                init_node = transform_expression(child)

        ast_children = [ast_leaf(type_name)]
        if init_node is not None:
            ast_children.append(init_node)

        return {"name": f"declare {identifier}", "children": ast_children}

    if name == "Assignment":
        if len(children) < 2:
            return {"name": "=", "children": []}

        return {
            "name": "=",
            "children": [
                transform_expression(children[0]),
                transform_expression(children[1]),
            ],
        }

    if name == "Print":
        return {
            "name": "print",
            "children": [transform_expression(child) for child in children],
        }

    if name == "Return":
        return {
            "name": "return",
            "children": [transform_expression(child) for child in children],
        }

    if name == "Block":
        block_children: list[dict[str, Any]] = []

        for child in children:
            transformed = transform_statement(child)
            if transformed is None:
                continue
            if transformed["name"] == "Program":
                block_children.extend(transformed["children"])
            else:
                block_children.append(transformed)

        return {"name": "block", "children": block_children}

    if name in {"If", "IfElse", "While"}:
        return {
            "name": name.lower(),
            "children": [
                transformed
                for child in children
                if (transformed := transform_statement(child) or transform_expression(child))
            ],
        }

    return transform_expression(node)


def build_ast(parse_tree: dict[str, Any]) -> dict[str, Any]:
    ast = transform_statement(parse_tree)
    return ast if ast is not None else {"name": "Program", "children": []}


def ast_source(source: str) -> dict[str, Any]:
    parser_result = parse_source(source)

    if not parser_result["success"]:
        return {
            "success": False,
            "status": "skipped",
            "parser_status": parser_result["status"],
            "message": "AST generation skipped because parsing failed.",
            "errors": parser_result["errors"],
            "ast": {"name": "Program", "children": []},
            "parse_tree": parser_result["parse_tree"],
            "tokens": parser_result["tokens"],
            "metrics": parser_result["metrics"],
        }

    return {
        "success": True,
        "status": "success",
        "parser_status": parser_result["status"],
        "message": "AST generated successfully.",
        "errors": [],
        "ast": build_ast(parser_result["parse_tree"]),
        "parse_tree": parser_result["parse_tree"],
        "tokens": parser_result["tokens"],
        "metrics": parser_result["metrics"],
    }


def semantic_error(message: str, node: dict[str, Any] | None = None) -> dict[str, Any]:
    error: dict[str, Any] = {"message": message}
    if node is not None:
        error["node"] = node.get("name", "")
    return error


def is_int_literal(value: str) -> bool:
    return re.fullmatch(r"\d+", value) is not None


def is_float_literal(value: str) -> bool:
    return re.fullmatch(r"\d+\.\d+", value) is not None


def is_string_literal(value: str) -> bool:
    return len(value) >= 2 and value.startswith('"') and value.endswith('"')


def is_char_literal(value: str) -> bool:
    return len(value) >= 3 and value.startswith("'") and value.endswith("'")


def lookup_symbol(name: str, scopes: list[dict[str, dict[str, Any]]]) -> dict[str, Any] | None:
    for scope in reversed(scopes):
        if name in scope:
            return scope[name]
    return None


def assignment_compatible(target_type: str, value_type: str) -> bool:
    if value_type == "error":
        return True
    if target_type == value_type:
        return True
    if target_type == "float" and value_type == "int":
        return True
    return False


class SemanticAnalyzer:
    def __init__(self) -> None:
        self.scopes: list[dict[str, dict[str, Any]]] = [{}]
        self.scope_names = ["global"]
        self.errors: list[dict[str, Any]] = []
        self.symbol_table: list[dict[str, Any]] = []
        self.next_address = 100
        self.block_counter = 0

    def current_scope(self) -> dict[str, dict[str, Any]]:
        return self.scopes[-1]

    def current_scope_name(self) -> str:
        return self.scope_names[-1]

    def enter_scope(self) -> None:
        self.block_counter += 1
        self.scopes.append({})
        self.scope_names.append(f"block{self.block_counter}")

    def exit_scope(self) -> None:
        self.scopes.pop()
        self.scope_names.pop()

    def declare(self, name: str, declared_type: str, node: dict[str, Any]) -> None:
        if name in self.current_scope():
            self.errors.append(semantic_error(f"Duplicate declaration of variable '{name}'.", node))
            return

        symbol = {
            "name": name,
            "type": declared_type,
            "scope": self.current_scope_name(),
            "address": self.next_address,
        }
        self.next_address += 4
        self.current_scope()[name] = symbol
        self.symbol_table.append(symbol)

    def infer_expression_type(self, node: dict[str, Any]) -> str:
        name = node.get("name", "")
        children = node.get("children", [])

        if name in {"+", "-", "*", "/", "%"}:
            left_type = self.infer_expression_type(children[0]) if children else "error"
            right_type = self.infer_expression_type(children[1]) if len(children) > 1 else "error"

            if left_type == "error" or right_type == "error":
                return "error"
            if left_type not in {"int", "float"} or right_type not in {"int", "float"}:
                self.errors.append(semantic_error(f"Operator '{name}' requires numeric operands.", node))
                return "error"
            if name == "/" or "float" in {left_type, right_type}:
                return "float"
            return "int"

        if name in {"<", ">", "<=", ">=", "==", "!=", "&&", "||"}:
            for child in children:
                self.infer_expression_type(child)
            return "bool"

        if name == "!":
            for child in children:
                self.infer_expression_type(child)
            return "bool"

        if is_int_literal(name):
            return "int"
        if is_float_literal(name):
            return "float"
        if is_string_literal(name):
            return "string"
        if is_char_literal(name):
            return "char"

        symbol = lookup_symbol(name, self.scopes)
        if symbol is None:
            self.errors.append(semantic_error(f"Variable '{name}' not declared.", node))
            return "error"

        return str(symbol["type"])

    def analyze_statement(self, node: dict[str, Any]) -> None:
        name = node.get("name", "")
        children = node.get("children", [])

        if name == "Program":
            for child in children:
                self.analyze_statement(child)
            return

        if name.startswith("declare "):
            variable_name = name.removeprefix("declare ").strip()
            declared_type = children[0].get("name", "unknown") if children else "unknown"
            self.declare(variable_name, declared_type, node)

            if len(children) > 1:
                value_type = self.infer_expression_type(children[1])
                if not assignment_compatible(declared_type, value_type):
                    self.errors.append(
                        semantic_error(
                            f"Cannot assign {value_type} to {declared_type} variable '{variable_name}'.",
                            node,
                        )
                    )
            return

        if name == "=":
            if len(children) < 2:
                self.errors.append(semantic_error("Invalid assignment.", node))
                return

            target_name = children[0].get("name", "")
            symbol = lookup_symbol(target_name, self.scopes)
            value_type = self.infer_expression_type(children[1])

            if symbol is None:
                self.errors.append(semantic_error(f"Variable '{target_name}' not declared.", children[0]))
                return

            if not assignment_compatible(str(symbol["type"]), value_type):
                self.errors.append(
                    semantic_error(
                        f"Cannot assign {value_type} to {symbol['type']} variable '{target_name}'.",
                        node,
                    )
                )
            return

        if name == "block":
            self.enter_scope()
            for child in children:
                self.analyze_statement(child)
            self.exit_scope()
            return

        if name in {"while", "if", "ifelse"}:
            if children:
                self.infer_expression_type(children[0])
            for child in children[1:]:
                self.analyze_statement(child)
            return

        if name in {"print", "return"}:
            for child in children:
                self.infer_expression_type(child)
            return

        self.infer_expression_type(node)


def semantic_source(source: str) -> dict[str, Any]:
    ast_result = ast_source(source)

    if not ast_result["success"]:
        return {
            "success": False,
            "status": "skipped",
            "message": "Semantic analysis skipped because AST generation failed.",
            "errors": ast_result["errors"],
            "symbol_table": [],
            "ast": ast_result["ast"],
            "parse_tree": ast_result["parse_tree"],
            "tokens": ast_result["tokens"],
            "metrics": ast_result["metrics"],
            "parser_status": ast_result["parser_status"],
        }

    analyzer = SemanticAnalyzer()
    analyzer.analyze_statement(ast_result["ast"])

    return {
        "success": len(analyzer.errors) == 0,
        "status": "success" if len(analyzer.errors) == 0 else "error",
        "message": "Semantic analysis successful." if len(analyzer.errors) == 0 else "Semantic analysis found errors.",
        "errors": analyzer.errors,
        "symbol_table": analyzer.symbol_table,
        "ast": ast_result["ast"],
        "parse_tree": ast_result["parse_tree"],
        "tokens": ast_result["tokens"],
        "metrics": ast_result["metrics"],
        "parser_status": ast_result["parser_status"],
    }


class IRGenerator:
    def __init__(self) -> None:
        self.instructions: list[str] = []
        self.temp_counter = 0
        self.label_counter = 0

    def new_temp(self) -> str:
        self.temp_counter += 1
        return f"t{self.temp_counter}"

    def new_label(self) -> str:
        self.label_counter += 1
        return f"L{self.label_counter}"

    def emit(self, instruction: str) -> None:
        self.instructions.append(instruction)

    def generate_expression(self, node: dict[str, Any]) -> str:
        name = node.get("name", "")
        children = node.get("children", [])

        if name in {"+", "-", "*", "/", "%", "<", ">", "<=", ">=", "==", "!=", "&&", "||"}:
            left = self.generate_expression(children[0]) if children else ""
            right = self.generate_expression(children[1]) if len(children) > 1 else ""
            temp = self.new_temp()
            self.emit(f"{temp} = {left} {name} {right}")
            return temp

        if name == "!":
            operand = self.generate_expression(children[0]) if children else ""
            temp = self.new_temp()
            self.emit(f"{temp} = ! {operand}")
            return temp

        return name

    def generate_statement(self, node: dict[str, Any]) -> None:
        name = node.get("name", "")
        children = node.get("children", [])

        if name == "Program":
            for child in children:
                self.generate_statement(child)
            return

        if name.startswith("declare "):
            variable_name = name.removeprefix("declare ").strip()
            if len(children) > 1:
                value = self.generate_expression(children[1])
                self.emit(f"{variable_name} = {value}")
            else:
                self.emit(f"declare {variable_name}")
            return

        if name == "=":
            if len(children) < 2:
                return
            target = children[0].get("name", "")
            value = self.generate_expression(children[1])
            self.emit(f"{target} = {value}")
            return

        if name == "print":
            for child in children:
                value = self.generate_expression(child)
                self.emit(f"print {value}")
            return

        if name == "return":
            value = self.generate_expression(children[0]) if children else ""
            self.emit(f"return {value}".strip())
            return

        if name == "block":
            for child in children:
                self.generate_statement(child)
            return

        if name == "while":
            start_label = self.new_label()
            end_label = self.new_label()
            self.emit(f"{start_label}:")
            condition = self.generate_expression(children[0]) if children else ""
            self.emit(f"ifFalse {condition} goto {end_label}")
            for child in children[1:]:
                self.generate_statement(child)
            self.emit(f"goto {start_label}")
            self.emit(f"{end_label}:")
            return

        if name in {"if", "ifelse"}:
            else_label = self.new_label()
            end_label = self.new_label()
            condition = self.generate_expression(children[0]) if children else ""
            self.emit(f"ifFalse {condition} goto {else_label}")
            if len(children) > 1:
                self.generate_statement(children[1])
            self.emit(f"goto {end_label}")
            self.emit(f"{else_label}:")
            if name == "ifelse" and len(children) > 2:
                self.generate_statement(children[2])
            self.emit(f"{end_label}:")


def ir_source(source: str) -> dict[str, Any]:
    semantic_result = semantic_source(source)

    if not semantic_result["success"]:
        return {
            "success": False,
            "status": "skipped",
            "message": "IR generation skipped because semantic analysis failed.",
            "errors": semantic_result["errors"],
            "ir": [],
            "ast": semantic_result["ast"],
            "parse_tree": semantic_result["parse_tree"],
            "tokens": semantic_result["tokens"],
            "metrics": semantic_result["metrics"],
            "parser_status": semantic_result["parser_status"],
            "semantic_status": semantic_result["status"],
            "symbol_table": semantic_result["symbol_table"],
        }

    generator = IRGenerator()
    generator.generate_statement(semantic_result["ast"])

    return {
        "success": True,
        "status": "success",
        "message": "Intermediate code generated successfully.",
        "errors": [],
        "ir": generator.instructions,
        "ast": semantic_result["ast"],
        "parse_tree": semantic_result["parse_tree"],
        "tokens": semantic_result["tokens"],
        "metrics": semantic_result["metrics"],
        "parser_status": semantic_result["parser_status"],
        "semantic_status": semantic_result["status"],
        "symbol_table": semantic_result["symbol_table"],
    }


def numeric_value(value: str) -> int | float | None:
    if is_int_literal(value):
        return int(value)
    if is_float_literal(value):
        return float(value)
    return None


def format_number(value: int | float) -> str:
    if isinstance(value, float) and value.is_integer():
        return str(int(value))
    return str(value)


def fold_binary(left: str, operator: str, right: str) -> str | None:
    left_value = numeric_value(left)
    right_value = numeric_value(right)

    if left_value is not None and right_value is not None:
        try:
            if operator == "+":
                return format_number(left_value + right_value)
            if operator == "-":
                return format_number(left_value - right_value)
            if operator == "*":
                return format_number(left_value * right_value)
            if operator == "/" and right_value != 0:
                return format_number(left_value / right_value)
            if operator == "%" and right_value != 0:
                return format_number(left_value % right_value)
        except ZeroDivisionError:
            return None

    if operator == "+" and right == "0":
        return left
    if operator == "+" and left == "0":
        return right
    if operator == "-" and right == "0":
        return left
    if operator == "*" and right == "1":
        return left
    if operator == "*" and left == "1":
        return right
    if operator == "*" and (left == "0" or right == "0"):
        return "0"
    if operator == "/" and right == "1":
        return left

    return None


def replace_known_copies(parts: list[str], copies: dict[str, str]) -> list[str]:
    return [copies.get(part, part) for part in parts]


def optimize_ir(instructions: list[str]) -> tuple[list[str], list[str]]:
    optimized: list[str] = []
    notes: list[str] = []
    copies: dict[str, str] = {}
    binary_pattern = re.compile(r"^(\w+) = ([^ ]+) ([+\-*/%]) ([^ ]+)$")
    copy_pattern = re.compile(r"^(\w+) = ([A-Za-z_]\w*|\d+(?:\.\d+)?)$")

    for instruction in instructions:
        if instruction.endswith(":") or instruction.startswith(("goto ", "ifFalse ")):
            copies.clear()
            optimized.append(instruction)
            continue

        if instruction.startswith(("print ", "return", "declare ")):
            optimized.append(instruction)
            continue

        binary_match = binary_pattern.match(instruction)
        if binary_match:
            target, left, operator, right = binary_match.groups()
            left, right = replace_known_copies([left, right], copies)
            folded = fold_binary(left, operator, right)

            if folded is not None:
                optimized.append(f"{target} = {folded}")
                copies[target] = folded
                notes.append(f"Constant/algebraic simplification: {instruction} -> {target} = {folded}")
            else:
                optimized.append(f"{target} = {left} {operator} {right}")
                copies.pop(target, None)
            continue

        copy_match = copy_pattern.match(instruction)
        if copy_match:
            target, value = copy_match.groups()
            value = copies.get(value, value)
            optimized.append(f"{target} = {value}")
            copies[target] = value
            if instruction != f"{target} = {value}":
                notes.append(f"Copy propagation: {instruction} -> {target} = {value}")
            continue

        optimized.append(instruction)

    optimized = remove_dead_stores(optimized, notes)
    return optimized, notes


def variable_tokens(instruction: str) -> set[str]:
    return set(re.findall(r"\b[A-Za-z_]\w*\b", instruction))


def remove_dead_stores(instructions: list[str], notes: list[str]) -> list[str]:
    result: list[str] = []
    pending_assignment_index: dict[str, int] = {}

    for instruction in instructions:
        assignment_match = re.match(r"^([A-Za-z_]\w*) = ", instruction)
        boundary = instruction.endswith(":") or instruction.startswith(("goto ", "ifFalse ", "print ", "return"))

        if boundary:
            for variable in variable_tokens(instruction):
                pending_assignment_index.pop(variable, None)
            pending_assignment_index.clear()
            result.append(instruction)
            continue

        if assignment_match:
            target = assignment_match.group(1)
            used_variables = variable_tokens(instruction.split("=", 1)[1])

            for variable in used_variables:
                pending_assignment_index.pop(variable, None)

            if target in pending_assignment_index:
                removed_index = pending_assignment_index[target]
                removed = result[removed_index]
                result[removed_index] = ""
                notes.append(f"Dead store eliminated: {removed}")

            pending_assignment_index[target] = len(result)
            result.append(instruction)
            continue

        for variable in variable_tokens(instruction):
            pending_assignment_index.pop(variable, None)
        result.append(instruction)

    return [instruction for instruction in result if instruction]


def optimize_source(source: str) -> dict[str, Any]:
    ir_result = ir_source(source)

    if not ir_result["success"]:
        return {
            **ir_result,
            "optimized_ir": [],
            "optimizations": [],
            "message": "Optimization skipped because IR generation failed.",
        }

    optimized_ir, notes = optimize_ir(ir_result["ir"])

    return {
        **ir_result,
        "message": "Optimization completed.",
        "optimized_ir": optimized_ir,
        "optimizations": notes,
    }


class AssemblyGenerator:
    def __init__(self) -> None:
        self.instructions: list[str] = []
        self.register_counter = 0
        self.allocated: set[str] = set()

    def new_register(self) -> str:
        self.register_counter += 1
        return f"R{self.register_counter}"

    def emit(self, instruction: str) -> None:
        self.instructions.append(instruction)

    def allocate(self, name: str) -> None:
        if not re.match(r"^[A-Za-z_]\w*$", name) or name in self.allocated:
            return

        self.allocated.add(name)
        self.emit(f"ALLOC {name}")

    def load_operand(self, operand: str) -> str:
        register = self.new_register()
        self.emit(f"MOV {register}, {operand}")
        return register

    def generate(self, ir: list[str], symbol_table: list[dict[str, Any]]) -> list[str]:
        binary_pattern = re.compile(r"^(\w+) = ([^ ]+) ([+\-*/%<>=!&|]+) ([^ ]+)$")
        unary_pattern = re.compile(r"^(\w+) = ! ([^ ]+)$")
        assignment_pattern = re.compile(r"^(\w+) = (.+)$")

        operation_map = {
            "+": "ADD",
            "-": "SUB",
            "*": "MUL",
            "/": "DIV",
            "%": "MOD",
            "<": "CMP_LT",
            ">": "CMP_GT",
            "<=": "CMP_LE",
            ">=": "CMP_GE",
            "==": "CMP_EQ",
            "!=": "CMP_NE",
            "&&": "AND",
            "||": "OR",
        }

        for symbol in symbol_table:
            self.allocate(str(symbol.get("name", "")))

        for instruction in ir:
            if instruction.endswith(":"):
                self.emit(instruction)
                continue

            if instruction.startswith("declare "):
                variable = instruction.removeprefix("declare ").strip()
                self.allocate(variable)
                continue

            if instruction.startswith("goto "):
                label = instruction.removeprefix("goto ").strip()
                self.emit(f"JMP {label}")
                continue

            if instruction.startswith("ifFalse "):
                match = re.match(r"^ifFalse (.+) goto (\w+)$", instruction)
                if match:
                    condition, label = match.groups()
                    condition_register = self.load_operand(condition)
                    self.emit(f"JZ {condition_register}, {label}")
                continue

            if instruction.startswith("print "):
                value = instruction.removeprefix("print ").strip()
                value_register = self.load_operand(value)
                self.emit(f"PRINT {value_register}")
                continue

            if instruction.startswith("return"):
                value = instruction.removeprefix("return").strip()
                if value:
                    value_register = self.load_operand(value)
                    self.emit(f"RET {value_register}")
                else:
                    self.emit("RET")
                continue

            binary_match = binary_pattern.match(instruction)
            if binary_match:
                target, left, operator, right = binary_match.groups()
                self.allocate(target)
                left_register = self.load_operand(left)
                right_register = self.load_operand(right)
                result_register = self.new_register()
                operation = operation_map.get(operator, "OP")
                self.emit(f"{operation} {result_register}, {left_register}, {right_register}")
                self.emit(f"STORE {target}, {result_register}")
                continue

            unary_match = unary_pattern.match(instruction)
            if unary_match:
                target, operand = unary_match.groups()
                self.allocate(target)
                operand_register = self.load_operand(operand)
                result_register = self.new_register()
                self.emit(f"NOT {result_register}, {operand_register}")
                self.emit(f"STORE {target}, {result_register}")
                continue

            assignment_match = assignment_pattern.match(instruction)
            if assignment_match:
                target, value = assignment_match.groups()
                self.allocate(target)
                value_register = self.load_operand(value)
                self.emit(f"STORE {target}, {value_register}")
                continue

            self.emit(f"; unsupported IR: {instruction}")

        return self.instructions


def assembly_source(source: str) -> dict[str, Any]:
    optimized_result = optimize_source(source)

    if not optimized_result["success"]:
        return {
            **optimized_result,
            "assembly": [],
            "message": "Assembly generation skipped because optimization failed.",
        }

    generator = AssemblyGenerator()
    assembly = generator.generate(optimized_result["optimized_ir"], optimized_result["symbol_table"])

    return {
        **optimized_result,
        "message": "Assembly generated successfully.",
        "assembly": assembly,
    }


class VirtualMachine:
    def __init__(self, max_steps: int = 1000) -> None:
        self.max_steps = max_steps
        self.registers: dict[str, int | float | str] = {}
        self.memory: dict[str, int | float | str | None] = {}
        self.stack: list[int | float | str] = []
        self.output: list[str] = []
        self.trace: list[dict[str, Any]] = []
        self.return_value: int | float | str | None = None
        self.halted = False
        self.errors: list[dict[str, Any]] = []

    def parse_value(self, value: str) -> int | float | str | None:
        value = value.strip()

        if value in self.registers:
            return self.registers[value]

        if value in self.memory:
            return self.memory[value]

        if is_int_literal(value):
            return int(value)

        if is_float_literal(value):
            return float(value)

        if len(value) >= 2 and value[0] == '"' and value[-1] == '"':
            return value[1:-1]

        if len(value) >= 2 and value[0] == "'" and value[-1] == "'":
            return value[1:-1]

        return value

    def numeric(self, value: int | float | str | None) -> int | float:
        if isinstance(value, int | float):
            return value

        if isinstance(value, str):
            parsed = numeric_value(value)
            if parsed is not None:
                return parsed

        return 0

    def truthy(self, value: int | float | str | None) -> bool:
        if value is None:
            return False
        if isinstance(value, int | float):
            return value != 0
        return bool(value)

    def split_operands(self, instruction: str) -> list[str]:
        return [part.strip() for part in instruction.split(" ", 1)[1].split(",")]

    def snapshot(self, pc: int, instruction: str) -> None:
        self.trace.append(
            {
                "pc": pc,
                "instruction": instruction,
                "registers": dict(self.registers),
                "memory": dict(self.memory),
                "output": list(self.output),
            }
        )

    def run(self, assembly: list[str]) -> dict[str, Any]:
        labels = {
            instruction.removesuffix(":"): index
            for index, instruction in enumerate(assembly)
            if instruction.endswith(":")
        }

        pc = 0
        steps = 0

        while pc < len(assembly) and not self.halted:
            if steps >= self.max_steps:
                self.errors.append({"message": f"VM stopped after {self.max_steps} steps."})
                break

            instruction = assembly[pc]
            steps += 1

            if not instruction or instruction.startswith(";"):
                pc += 1
                continue

            if instruction.endswith(":"):
                self.snapshot(pc, instruction)
                pc += 1
                continue

            opcode = instruction.split(" ", 1)[0]
            next_pc = pc + 1

            try:
                if opcode == "ALLOC":
                    name = instruction.split(" ", 1)[1].strip()
                    self.memory.setdefault(name, None)

                elif opcode == "MOV":
                    target, value = self.split_operands(instruction)
                    self.registers[target] = self.parse_value(value)

                elif opcode == "STORE":
                    target, value = self.split_operands(instruction)
                    self.memory[target] = self.parse_value(value)

                elif opcode in {"ADD", "SUB", "MUL", "DIV", "MOD"}:
                    target, left, right = self.split_operands(instruction)
                    left_value = self.numeric(self.parse_value(left))
                    right_value = self.numeric(self.parse_value(right))
                    if opcode == "ADD":
                        result = left_value + right_value
                    elif opcode == "SUB":
                        result = left_value - right_value
                    elif opcode == "MUL":
                        result = left_value * right_value
                    elif opcode == "DIV":
                        result = 0 if right_value == 0 else left_value / right_value
                    else:
                        result = 0 if right_value == 0 else left_value % right_value
                    self.registers[target] = result

                elif opcode.startswith("CMP_"):
                    target, left, right = self.split_operands(instruction)
                    left_value = self.numeric(self.parse_value(left))
                    right_value = self.numeric(self.parse_value(right))
                    comparisons = {
                        "CMP_LT": left_value < right_value,
                        "CMP_GT": left_value > right_value,
                        "CMP_LE": left_value <= right_value,
                        "CMP_GE": left_value >= right_value,
                        "CMP_EQ": left_value == right_value,
                        "CMP_NE": left_value != right_value,
                    }
                    self.registers[target] = 1 if comparisons.get(opcode, False) else 0

                elif opcode in {"AND", "OR"}:
                    target, left, right = self.split_operands(instruction)
                    left_value = self.truthy(self.parse_value(left))
                    right_value = self.truthy(self.parse_value(right))
                    self.registers[target] = 1 if (left_value and right_value if opcode == "AND" else left_value or right_value) else 0

                elif opcode == "NOT":
                    target, value = self.split_operands(instruction)
                    self.registers[target] = 0 if self.truthy(self.parse_value(value)) else 1

                elif opcode == "JMP":
                    label = instruction.split(" ", 1)[1].strip()
                    next_pc = labels[label]

                elif opcode == "JZ":
                    condition, label = self.split_operands(instruction)
                    if not self.truthy(self.parse_value(condition)):
                        next_pc = labels[label]

                elif opcode == "PRINT":
                    value = instruction.split(" ", 1)[1].strip()
                    self.output.append(str(self.parse_value(value)))

                elif opcode == "RET":
                    value = instruction.removeprefix("RET").strip()
                    self.return_value = self.parse_value(value) if value else None
                    if self.return_value is not None:
                        self.stack.append(self.return_value)
                    self.halted = True

                else:
                    self.errors.append({"message": f"Unsupported VM instruction: {instruction}", "pc": pc})

            except (KeyError, ValueError, IndexError) as exc:
                self.errors.append({"message": f"VM error at instruction '{instruction}': {exc}", "pc": pc})
                self.halted = True

            self.snapshot(pc, instruction)
            pc = next_pc

        return {
            "status": "success" if not self.errors else "error",
            "steps": steps,
            "registers": self.registers,
            "memory": self.memory,
            "stack": self.stack,
            "output": self.output,
            "trace": self.trace,
            "return_value": self.return_value,
            "errors": self.errors,
        }


def vm_source(source: str) -> dict[str, Any]:
    assembly_result = assembly_source(source)

    if not assembly_result["success"]:
        return {
            **assembly_result,
            "vm": {
                "status": "skipped",
                "steps": 0,
                "registers": {},
                "memory": {},
                "stack": [],
                "output": [],
                "trace": [],
                "return_value": None,
                "errors": assembly_result["errors"],
            },
            "message": "VM execution skipped because assembly generation failed.",
        }

    vm = VirtualMachine()
    vm_result = vm.run(assembly_result["assembly"])

    return {
        **assembly_result,
        "message": "VM execution completed." if vm_result["status"] == "success" else "VM execution found errors.",
        "vm": vm_result,
    }


def empty_ai_explanation() -> dict[str, Any]:
    return {
        "status": "success",
        "summary": "No compiler errors were found.",
        "likely_cause": "The source passed the current compiler pipeline.",
        "suggested_fix": "No fix is needed.",
        "example": "",
        "errors": [],
    }


def normalize_error_message(error: str | dict[str, Any]) -> str:
    if isinstance(error, dict):
        message = error.get("message", error)
        return str(message)

    return str(error)


def explain_error_message(message: str) -> dict[str, str]:
    lowered = message.lower()

    if "expected ;" in lowered or "semicolon" in lowered:
        return {
            "summary": "A statement is missing its terminating semicolon.",
            "likely_cause": "Tiny C statements such as declarations, assignments, print calls, and return statements must end with ';'.",
            "suggested_fix": "Add a semicolon at the end of the statement reported by the parser.",
            "example": "int a = 5;",
        }

    if "not declared" in lowered:
        return {
            "summary": "The program is using a variable before declaring it.",
            "likely_cause": "Semantic analysis could not find the variable in the current or parent scope.",
            "suggested_fix": "Declare the variable with a type before assigning to it or printing it.",
            "example": "int x;\nx = 5;",
        }

    if "duplicate declaration" in lowered:
        return {
            "summary": "A variable was declared more than once in the same scope.",
            "likely_cause": "The symbol table already has an entry for that name in the current block.",
            "suggested_fix": "Rename one variable or remove the duplicate declaration.",
            "example": "int count;\ncount = 1;",
        }

    if "cannot assign" in lowered or "type" in lowered:
        return {
            "summary": "The assigned value does not match the variable type.",
            "likely_cause": "Semantic analysis found an incompatible assignment.",
            "suggested_fix": "Change the variable type or assign a value of the expected type.",
            "example": "int a = 5;\nfloat b = 5.5;",
        }

    if "lexical" in lowered or "unknown" in lowered:
        return {
            "summary": "The lexer found text that is not valid Tiny C syntax.",
            "likely_cause": "The source contains an unsupported character, token, or malformed literal.",
            "suggested_fix": "Check the highlighted line and replace unsupported syntax with Tiny C tokens.",
            "example": "print(a);",
        }

    if "vm" in lowered:
        return {
            "summary": "The virtual machine could not execute one generated instruction.",
            "likely_cause": "The assembly stage produced an instruction the VM does not support yet, or execution exceeded the step limit.",
            "suggested_fix": "Inspect the Assembly and VM tabs to find the failing instruction.",
            "example": "while (a < 10) { a = a + 1; }",
        }

    return {
        "summary": "The compiler pipeline found an error.",
        "likely_cause": "The source does not match a rule enforced by the current compiler stage.",
        "suggested_fix": "Open the Parser, Semantic, and VM tabs to inspect the exact failing stage.",
        "example": "int a = 5;\nprint(a);",
    }


def explain_errors(errors: list[str | dict[str, Any]]) -> dict[str, Any]:
    if not errors:
        return empty_ai_explanation()

    first_message = normalize_error_message(errors[0])
    explanation = explain_error_message(first_message)

    return {
        "status": "error",
        "summary": explanation["summary"],
        "likely_cause": explanation["likely_cause"],
        "suggested_fix": explanation["suggested_fix"],
        "example": explanation["example"],
        "errors": errors,
    }


def explain_source(source: str) -> dict[str, Any]:
    result = compile_source(source)
    return {
        "success": True,
        "message": "AI-style error explanation generated.",
        "ai_explanation": result["ai_explanation"],
    }


def compile_source(source: str) -> dict[str, Any]:
    normalized_source = source.strip()

    if not normalized_source:
        return CompilationResult(
            success=False,
            message="No source code provided.",
            tokens=[],
            parser={"status": "skipped", "errors": ["Source code is empty."]},
            parse_tree={"name": "Program", "children": []},
            ast={"name": "Program", "children": []},
            semantic={"status": "skipped", "errors": []},
            symbol_table=[],
            ir=[],
            optimized_ir=[],
            optimizations=[],
            assembly=[],
            vm={
                "status": "skipped",
                "steps": 0,
                "registers": {},
                "memory": {},
                "stack": [],
                "output": [],
                "trace": [],
                "return_value": None,
                "errors": [],
            },
            ai_explanation=explain_errors([{"message": "Source code is empty.", "line": 1, "column": 1}]),
            metrics=empty_metrics(),
        ).to_dict()

    vm_result = vm_source(source)
    tokens = vm_result["tokens"]

    pipeline_errors = vm_result.get("errors", [])
    vm_errors = vm_result.get("vm", {}).get("errors", [])
    ai_explanation = explain_errors([*pipeline_errors, *vm_errors])

    return CompilationResult(
        success=vm_result["success"] and vm_result["vm"]["status"] == "success",
        message="Phase 10 standout features completed.",
        tokens=tokens,
        parser={
            "status": vm_result["parser_status"],
            "errors": [] if vm_result["parser_status"] == "success" else vm_result["errors"],
        },
        parse_tree=vm_result["parse_tree"],
        ast=vm_result["ast"],
        semantic={"status": vm_result["semantic_status"], "errors": vm_result["errors"]},
        symbol_table=vm_result["symbol_table"],
        ir=vm_result["ir"],
        optimized_ir=vm_result["optimized_ir"],
        optimizations=vm_result.get("optimizations", []),
        assembly=vm_result["assembly"],
        vm=vm_result["vm"],
        ai_explanation=ai_explanation,
        metrics=vm_result["metrics"],
    ).to_dict()
