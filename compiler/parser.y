%{
#include <cctype>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

struct Node {
    std::string name;
    std::vector<std::unique_ptr<Node>> children;

    explicit Node(std::string node_name) : name(std::move(node_name)) {}
};

struct TokenValue {
    std::string lexeme;
    int line;
    int column;
};

std::string source;
std::size_t cursor = 0;
int current_line = 1;
int current_column = 1;
TokenValue current_token;
std::unique_ptr<Node> parse_tree;
std::string syntax_error;
bool parser_had_error = false;

int yylex();
void yyerror(const char *message);

std::unique_ptr<Node> node(const std::string &name) {
    return std::make_unique<Node>(name);
}

std::unique_ptr<Node> node(const std::string &name, std::vector<std::unique_ptr<Node>> children) {
    auto result = std::make_unique<Node>(name);
    result->children = std::move(children);
    return result;
}

void append_child(std::vector<std::unique_ptr<Node>> &children, std::unique_ptr<Node> child) {
    if (child) {
        children.push_back(std::move(child));
    }
}

std::string json_escape(const std::string &text) {
    std::string escaped;

    for (char ch : text) {
        switch (ch) {
            case '\\':
                escaped += "\\\\";
                break;
            case '"':
                escaped += "\\\"";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            default:
                escaped += ch;
                break;
        }
    }

    return escaped;
}

void print_node_json(const Node *tree_node) {
    std::cout << "{\"name\":\"" << json_escape(tree_node->name) << "\",\"children\":[";

    for (std::size_t index = 0; index < tree_node->children.size(); ++index) {
        if (index > 0) {
            std::cout << ",";
        }
        print_node_json(tree_node->children[index].get());
    }

    std::cout << "]}";
}

void advance_position(char ch) {
    if (ch == '\n') {
        current_line++;
        current_column = 1;
    } else {
        current_column++;
    }
}

void skip_whitespace_and_comments() {
    while (cursor < source.size()) {
        char ch = source[cursor];

        if (std::isspace(static_cast<unsigned char>(ch))) {
            advance_position(ch);
            cursor++;
            continue;
        }

        if (ch == '/' && cursor + 1 < source.size() && source[cursor + 1] == '/') {
            while (cursor < source.size() && source[cursor] != '\n') {
                advance_position(source[cursor]);
                cursor++;
            }
            continue;
        }

        if (ch == '/' && cursor + 1 < source.size() && source[cursor + 1] == '*') {
            advance_position(source[cursor++]);
            advance_position(source[cursor++]);

            while (cursor + 1 < source.size() && !(source[cursor] == '*' && source[cursor + 1] == '/')) {
                advance_position(source[cursor]);
                cursor++;
            }

            if (cursor + 1 < source.size()) {
                advance_position(source[cursor++]);
                advance_position(source[cursor++]);
            }
            continue;
        }

        break;
    }
}
%}

%union {
    Node *node;
}

%token TOKEN_INT TOKEN_FLOAT TOKEN_CHAR TOKEN_STRING TOKEN_VOID
%token TOKEN_IF TOKEN_ELSE TOKEN_WHILE TOKEN_FOR TOKEN_RETURN TOKEN_PRINT
%token <node> TOKEN_IDENTIFIER TOKEN_INTEGER TOKEN_FLOAT_LITERAL TOKEN_STRING_LITERAL TOKEN_CHAR_LITERAL
%token TOKEN_EQ TOKEN_NE TOKEN_LE TOKEN_GE TOKEN_AND TOKEN_OR

%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_EQ TOKEN_NE
%left '<' '>' TOKEN_LE TOKEN_GE
%left '+' '-'
%left '*' '/' '%'
%right '!'
%nonassoc LOWER_THAN_ELSE
%nonassoc TOKEN_ELSE

%type <node> program statement_list statement declaration assignment print_statement
%type <node> block if_statement while_statement return_statement expression type literal

%%

program
    : statement_list
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          parse_tree = node("Program", std::move(children));
      }
    ;

statement_list
    : statement_list statement
      {
          auto children = std::move($1->children);
          delete $1;
          append_child(children, std::unique_ptr<Node>($2));
          $$ = node("Statements", std::move(children)).release();
      }
    | statement
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          $$ = node("Statements", std::move(children)).release();
      }
    ;

statement
    : declaration ';'
      {
          $$ = $1;
      }
    | assignment ';'
      {
          $$ = $1;
      }
    | print_statement ';'
      {
          $$ = $1;
      }
    | return_statement ';'
      {
          $$ = $1;
      }
    | if_statement
      {
          $$ = $1;
      }
    | while_statement
      {
          $$ = $1;
      }
    | block
      {
          $$ = $1;
      }
    ;

declaration
    : type TOKEN_IDENTIFIER
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($2));
          $$ = node("Declaration", std::move(children)).release();
      }
    | type TOKEN_IDENTIFIER '=' expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($2));
          append_child(children, std::unique_ptr<Node>($4));
          $$ = node("Declaration", std::move(children)).release();
      }
    ;

assignment
    : TOKEN_IDENTIFIER '=' expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Assignment", std::move(children)).release();
      }
    ;

print_statement
    : TOKEN_PRINT '(' expression ')'
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Print", std::move(children)).release();
      }
    ;

return_statement
    : TOKEN_RETURN expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($2));
          $$ = node("Return", std::move(children)).release();
      }
    ;

if_statement
    : TOKEN_IF '(' expression ')' statement %prec LOWER_THAN_ELSE
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($3));
          append_child(children, std::unique_ptr<Node>($5));
          $$ = node("If", std::move(children)).release();
      }
    | TOKEN_IF '(' expression ')' statement TOKEN_ELSE statement
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($3));
          append_child(children, std::unique_ptr<Node>($5));
          append_child(children, std::unique_ptr<Node>($7));
          $$ = node("IfElse", std::move(children)).release();
      }
    ;

while_statement
    : TOKEN_WHILE '(' expression ')' statement
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($3));
          append_child(children, std::unique_ptr<Node>($5));
          $$ = node("While", std::move(children)).release();
      }
    ;

block
    : '{' statement_list '}'
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($2));
          $$ = node("Block", std::move(children)).release();
      }
    ;

type
    : TOKEN_INT
      {
          $$ = node("Type: int").release();
      }
    | TOKEN_FLOAT
      {
          $$ = node("Type: float").release();
      }
    | TOKEN_CHAR
      {
          $$ = node("Type: char").release();
      }
    | TOKEN_STRING
      {
          $$ = node("Type: string").release();
      }
    | TOKEN_VOID
      {
          $$ = node("Type: void").release();
      }
    ;

expression
    : expression '+' expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: +", std::move(children)).release();
      }
    | expression '-' expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: -", std::move(children)).release();
      }
    | expression '*' expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: *", std::move(children)).release();
      }
    | expression '/' expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: /", std::move(children)).release();
      }
    | expression '%' expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: %", std::move(children)).release();
      }
    | expression TOKEN_EQ expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: ==", std::move(children)).release();
      }
    | expression TOKEN_NE expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: !=", std::move(children)).release();
      }
    | expression '<' expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: <", std::move(children)).release();
      }
    | expression '>' expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: >", std::move(children)).release();
      }
    | expression TOKEN_LE expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: <=", std::move(children)).release();
      }
    | expression TOKEN_GE expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: >=", std::move(children)).release();
      }
    | expression TOKEN_AND expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: &&", std::move(children)).release();
      }
    | expression TOKEN_OR expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($1));
          append_child(children, std::unique_ptr<Node>($3));
          $$ = node("Operator: ||", std::move(children)).release();
      }
    | '!' expression
      {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>($2));
          $$ = node("Operator: !", std::move(children)).release();
      }
    | '(' expression ')'
      {
          $$ = $2;
      }
    | TOKEN_IDENTIFIER
      {
          $$ = $1;
      }
    | literal
      {
          $$ = $1;
      }
    ;

literal
    : TOKEN_INTEGER
      {
          $$ = $1;
      }
    | TOKEN_FLOAT_LITERAL
      {
          $$ = $1;
      }
    | TOKEN_STRING_LITERAL
      {
          $$ = $1;
      }
    | TOKEN_CHAR_LITERAL
      {
          $$ = $1;
      }
    ;

%%

int yylex() {
    skip_whitespace_and_comments();

    if (cursor >= source.size()) {
        current_token = {"", current_line, current_column};
        return 0;
    }

    int token_line = current_line;
    int token_column = current_column;
    char ch = source[cursor];

    if (std::isalpha(static_cast<unsigned char>(ch)) || ch == '_') {
        std::size_t start = cursor;

        while (cursor < source.size()) {
            char next = source[cursor];
            if (!std::isalnum(static_cast<unsigned char>(next)) && next != '_') {
                break;
            }
            advance_position(next);
            cursor++;
        }

        std::string lexeme = source.substr(start, cursor - start);
        current_token = {lexeme, token_line, token_column};

        if (lexeme == "int") return TOKEN_INT;
        if (lexeme == "float") return TOKEN_FLOAT;
        if (lexeme == "char") return TOKEN_CHAR;
        if (lexeme == "string") return TOKEN_STRING;
        if (lexeme == "void") return TOKEN_VOID;
        if (lexeme == "if") return TOKEN_IF;
        if (lexeme == "else") return TOKEN_ELSE;
        if (lexeme == "while") return TOKEN_WHILE;
        if (lexeme == "for") return TOKEN_FOR;
        if (lexeme == "return") return TOKEN_RETURN;
        if (lexeme == "print") return TOKEN_PRINT;

        yylval.node = node("Identifier: " + lexeme).release();
        return TOKEN_IDENTIFIER;
    }

    if (std::isdigit(static_cast<unsigned char>(ch))) {
        std::size_t start = cursor;
        bool is_float = false;

        while (cursor < source.size() && std::isdigit(static_cast<unsigned char>(source[cursor]))) {
            advance_position(source[cursor]);
            cursor++;
        }

        if (cursor < source.size() && source[cursor] == '.') {
            is_float = true;
            advance_position(source[cursor]);
            cursor++;

            while (cursor < source.size() && std::isdigit(static_cast<unsigned char>(source[cursor]))) {
                advance_position(source[cursor]);
                cursor++;
            }
        }

        current_token = {source.substr(start, cursor - start), token_line, token_column};
        yylval.node = node(std::string(is_float ? "Float: " : "Integer: ") + current_token.lexeme).release();
        return is_float ? TOKEN_FLOAT_LITERAL : TOKEN_INTEGER;
    }

    if (ch == '"') {
        std::size_t start = cursor;
        advance_position(source[cursor++]);

        while (cursor < source.size() && source[cursor] != '"' && source[cursor] != '\n') {
            if (source[cursor] == '\\' && cursor + 1 < source.size()) {
                advance_position(source[cursor++]);
            }
            advance_position(source[cursor++]);
        }

        if (cursor < source.size() && source[cursor] == '"') {
            advance_position(source[cursor++]);
        }

        current_token = {source.substr(start, cursor - start), token_line, token_column};
        yylval.node = node("String: " + current_token.lexeme).release();
        return TOKEN_STRING_LITERAL;
    }

    if (ch == '\'') {
        std::size_t start = cursor;
        advance_position(source[cursor++]);

        while (cursor < source.size() && source[cursor] != '\'' && source[cursor] != '\n') {
            if (source[cursor] == '\\' && cursor + 1 < source.size()) {
                advance_position(source[cursor++]);
            }
            advance_position(source[cursor++]);
        }

        if (cursor < source.size() && source[cursor] == '\'') {
            advance_position(source[cursor++]);
        }

        current_token = {source.substr(start, cursor - start), token_line, token_column};
        yylval.node = node("Character: " + current_token.lexeme).release();
        return TOKEN_CHAR_LITERAL;
    }

    if (cursor + 1 < source.size()) {
        std::string two = source.substr(cursor, 2);
        current_token = {two, token_line, token_column};

        if (two == "==") {
            advance_position(source[cursor++]);
            advance_position(source[cursor++]);
            return TOKEN_EQ;
        }
        if (two == "!=") {
            advance_position(source[cursor++]);
            advance_position(source[cursor++]);
            return TOKEN_NE;
        }
        if (two == "<=") {
            advance_position(source[cursor++]);
            advance_position(source[cursor++]);
            return TOKEN_LE;
        }
        if (two == ">=") {
            advance_position(source[cursor++]);
            advance_position(source[cursor++]);
            return TOKEN_GE;
        }
        if (two == "&&") {
            advance_position(source[cursor++]);
            advance_position(source[cursor++]);
            return TOKEN_AND;
        }
        if (two == "||") {
            advance_position(source[cursor++]);
            advance_position(source[cursor++]);
            return TOKEN_OR;
        }
    }

    current_token = {std::string(1, ch), token_line, token_column};
    advance_position(ch);
    cursor++;

    return ch;
}

void yyerror(const char *message) {
    parser_had_error = true;

    std::ostringstream error;
    error << message << " near '" << current_token.lexeme << "'";
    syntax_error = error.str();
}

int main() {
    std::ostringstream buffer;
    buffer << std::cin.rdbuf();
    source = buffer.str();

    int result = yyparse();
    bool success = result == 0 && !parser_had_error;

    std::cout << "{\"success\":" << (success ? "true" : "false");
    std::cout << ",\"status\":\"" << (success ? "success" : "error") << "\"";
    std::cout << ",\"errors\":[";

    if (!success) {
        std::cout << "{\"message\":\"" << json_escape(syntax_error.empty() ? "Syntax error" : syntax_error)
                  << "\",\"line\":" << current_token.line
                  << ",\"column\":" << current_token.column
                  << "}";
    }

    std::cout << "],\"parse_tree\":";

    if (success && parse_tree) {
        print_node_json(parse_tree.get());
    } else {
        std::cout << "{\"name\":\"Program\",\"children\":[]}";
    }

    std::cout << "}";

    return success ? 0 : 1;
}
