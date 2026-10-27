CXX ?= clang++
FLEX ?= flex
BISON ?= bison

BUILD_DIR := build
LEXER_SRC := $(BUILD_DIR)/lex.yy.cc
LEXER_BIN := $(BUILD_DIR)/lexer
PARSER_SRC := $(BUILD_DIR)/parser.tab.cc
PARSER_BIN := $(BUILD_DIR)/parser

.PHONY: all lexer parser clean

all: lexer parser

lexer: $(LEXER_BIN)

parser: $(PARSER_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LEXER_SRC): lexer.l | $(BUILD_DIR)
	$(FLEX) -o $(LEXER_SRC) lexer.l

$(LEXER_BIN): $(LEXER_SRC)
	$(CXX) -std=c++17 -Wall -Wextra -o $(LEXER_BIN) $(LEXER_SRC)

$(PARSER_SRC): parser.y | $(BUILD_DIR)
	$(BISON) -d -o $(PARSER_SRC) parser.y

$(PARSER_BIN): $(PARSER_SRC)
	$(CXX) -std=c++17 -Wall -Wextra -o $(PARSER_BIN) $(PARSER_SRC)

clean:
	rm -rf $(BUILD_DIR)
