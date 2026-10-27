%option noyywrap

%{
#include <iostream>
#include <sstream>
#include <string>

int current_line = 1;
int current_column = 1;
bool first_token = true;
bool first_error = true;
bool has_error = false;
std::ostringstream errors_json;

std::string json_escape(const char *text) {
    std::string escaped;

    for (const char *cursor = text; *cursor != '\0'; ++cursor) {
        switch (*cursor) {
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
                escaped += *cursor;
                break;
        }
    }

    return escaped;
}

void advance_position(const char *text, int length) {
    for (int i = 0; i < length; ++i) {
        if (text[i] == '\n') {
            current_line++;
            current_column = 1;
        } else {
            current_column++;
        }
    }
}

void emit_token(const char *type, const char *lexeme, int length) {
    if (!first_token) {
        std::cout << ",";
    }

    std::cout
        << "{\"lexeme\":\"" << json_escape(lexeme)
        << "\",\"type\":\"" << type
        << "\",\"line\":" << current_line
        << ",\"column\":" << current_column
        << "}";

    first_token = false;
    advance_position(lexeme, length);
}

void emit_error(const char *message, const char *lexeme, int length) {
    errors_json
        << (first_error ? "" : ",")
        << "{\"message\":\"" << json_escape(message)
        << "\",\"lexeme\":\"" << json_escape(lexeme)
        << "\",\"line\":" << current_line
        << ",\"column\":" << current_column
        << "}";

    first_error = false;
    has_error = true;
    advance_position(lexeme, length);
}
%}

DIGIT       [0-9]
LETTER      [A-Za-z_]
IDENT       {LETTER}({LETTER}|{DIGIT})*
INT         {DIGIT}+
FLOAT       {DIGIT}+"."{DIGIT}+
STRING      \"([^\"\\\n]|\\.)*\"
CHAR        \'([^\'\\\n]|\\.)\'
KEYWORD     int|float|char|string|void|if|else|while|for|return|print|break|continue
OPERATOR    \+\+|--|==|!=|<=|>=|&&|\|\||\+=|-=|\*=|\/=|%=|<<|>>|[+\-*\/%=<>!&|]
DELIMITER   [;,(){}\[\]]

%%

[ \t\r]+                    { advance_position(yytext, yyleng); }
\n                          { advance_position(yytext, yyleng); }
"//"[^\n]*                  { advance_position(yytext, yyleng); }
"/*"([^*]|\*+[^*/])*\*+"/"  { advance_position(yytext, yyleng); }
"/*"([^*]|\*+[^*/])*        { emit_error("Unterminated block comment", yytext, yyleng); }

{KEYWORD}                   { emit_token("Keyword", yytext, yyleng); }
{FLOAT}                     { emit_token("Float", yytext, yyleng); }
{INT}                       { emit_token("Integer", yytext, yyleng); }
{STRING}                    { emit_token("String", yytext, yyleng); }
{CHAR}                      { emit_token("Character", yytext, yyleng); }
{IDENT}                     { emit_token("Identifier", yytext, yyleng); }
{OPERATOR}                  { emit_token("Operator", yytext, yyleng); }
{DELIMITER}                 { emit_token("Delimiter", yytext, yyleng); }
\"([^\"\\\n]|\\.)*          { emit_error("Unterminated string literal", yytext, yyleng); }
.                           { emit_error("Unexpected character", yytext, yyleng); }

%%

int main() {
    std::cout << "{\"tokens\":[";

    yylex();

    std::cout << "],\"success\":" << (has_error ? "false" : "true") << ",\"errors\":";
    std::cout << "[" << errors_json.str() << "]";
    std::cout << "}";

    return has_error ? 1 : 0;
}
