from __future__ import annotations

from flask import Flask, jsonify, request
from flask_cors import CORS

from compiler_service import (
    assembly_source,
    ast_source,
    compile_source,
    explain_source,
    ir_source,
    lex_source,
    optimize_source,
    parse_source,
    semantic_source,
    vm_source,
)


app = Flask(__name__)
CORS(app)


@app.get("/health")
def health() -> tuple[dict[str, str], int]:
    return {"status": "ok", "service": "CompilerViz API"}, 200


@app.post("/compile")
def compile_program():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")

    if not isinstance(source, str):
        return jsonify({"success": False, "message": "Field 'source' must be a string."}), 400

    return jsonify(compile_source(source))


@app.post("/lexer")
def lexer():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")

    if not isinstance(source, str):
        return jsonify({"success": False, "message": "Field 'source' must be a string."}), 400

    return jsonify(lex_source(source))


@app.post("/parser")
def parser():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")

    if not isinstance(source, str):
        return jsonify({"success": False, "message": "Field 'source' must be a string."}), 400

    return jsonify(parse_source(source))


@app.post("/ast")
def ast():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")

    if not isinstance(source, str):
        return jsonify({"success": False, "message": "Field 'source' must be a string."}), 400

    return jsonify(ast_source(source))


@app.post("/semantic")
def semantic():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")

    if not isinstance(source, str):
        return jsonify({"success": False, "message": "Field 'source' must be a string."}), 400

    return jsonify(semantic_source(source))


@app.post("/symbol-table")
def symbol_table():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")

    if not isinstance(source, str):
        return jsonify({"success": False, "message": "Field 'source' must be a string."}), 400

    result = semantic_source(source)
    return jsonify(
        {
            "success": result["success"],
            "status": result["status"],
            "message": result["message"],
            "errors": result["errors"],
            "symbol_table": result["symbol_table"],
        }
    )


@app.post("/ir")
def ir():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")

    if not isinstance(source, str):
        return jsonify({"success": False, "message": "Field 'source' must be a string."}), 400

    return jsonify(ir_source(source))


@app.post("/optimize")
def optimize():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")

    if not isinstance(source, str):
        return jsonify({"success": False, "message": "Field 'source' must be a string."}), 400

    return jsonify(optimize_source(source))


@app.post("/assembly")
def assembly():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")

    if not isinstance(source, str):
        return jsonify({"success": False, "message": "Field 'source' must be a string."}), 400

    return jsonify(assembly_source(source))


@app.post("/vm")
def vm():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")

    if not isinstance(source, str):
        return jsonify({"success": False, "message": "Field 'source' must be a string."}), 400

    return jsonify(vm_source(source))


@app.post("/explain")
def explain():
    payload = request.get_json(silent=True) or {}
    source = payload.get("source", "")

    if not isinstance(source, str):
        return jsonify({"success": False, "message": "Field 'source' must be a string."}), 400

    return jsonify(explain_source(source))


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5003, debug=False, use_reloader=False)
