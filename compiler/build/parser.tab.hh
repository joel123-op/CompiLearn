/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_INT = 258,
     TOKEN_FLOAT = 259,
     TOKEN_CHAR = 260,
     TOKEN_STRING = 261,
     TOKEN_VOID = 262,
     TOKEN_IF = 263,
     TOKEN_ELSE = 264,
     TOKEN_WHILE = 265,
     TOKEN_FOR = 266,
     TOKEN_RETURN = 267,
     TOKEN_PRINT = 268,
     TOKEN_IDENTIFIER = 269,
     TOKEN_INTEGER = 270,
     TOKEN_FLOAT_LITERAL = 271,
     TOKEN_STRING_LITERAL = 272,
     TOKEN_CHAR_LITERAL = 273,
     TOKEN_EQ = 274,
     TOKEN_NE = 275,
     TOKEN_LE = 276,
     TOKEN_GE = 277,
     TOKEN_AND = 278,
     TOKEN_OR = 279,
     LOWER_THAN_ELSE = 280
   };
#endif
/* Tokens.  */
#define TOKEN_INT 258
#define TOKEN_FLOAT 259
#define TOKEN_CHAR 260
#define TOKEN_STRING 261
#define TOKEN_VOID 262
#define TOKEN_IF 263
#define TOKEN_ELSE 264
#define TOKEN_WHILE 265
#define TOKEN_FOR 266
#define TOKEN_RETURN 267
#define TOKEN_PRINT 268
#define TOKEN_IDENTIFIER 269
#define TOKEN_INTEGER 270
#define TOKEN_FLOAT_LITERAL 271
#define TOKEN_STRING_LITERAL 272
#define TOKEN_CHAR_LITERAL 273
#define TOKEN_EQ 274
#define TOKEN_NE 275
#define TOKEN_LE 276
#define TOKEN_GE 277
#define TOKEN_AND 278
#define TOKEN_OR 279
#define LOWER_THAN_ELSE 280




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 140 "parser.y"
{
    Node *node;
}
/* Line 1529 of yacc.c.  */
#line 103 "build/parser.tab.hh"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

