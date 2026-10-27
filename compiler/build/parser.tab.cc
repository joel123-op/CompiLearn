/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "parser.y"

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


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 140 "parser.y"
{
    Node *node;
}
/* Line 193 of yacc.c.  */
#line 289 "build/parser.tab.cc"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 302 "build/parser.tab.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  37
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   203

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  14
/* YYNRULES -- Number of rules.  */
#define YYNRULES  46
/* YYNRULES -- Number of states.  */
#define YYNSTATES  87

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   280

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    32,     2,     2,     2,    31,     2,     2,
      36,    37,    29,    27,     2,    28,     2,    30,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    34,
      25,    35,    26,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    38,     2,    39,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      33
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    13,    16,    19,    22,
      24,    26,    28,    31,    36,    40,    45,    48,    54,    62,
      68,    72,    74,    76,    78,    80,    82,    86,    90,    94,
      98,   102,   106,   110,   114,   118,   122,   126,   130,   134,
     137,   141,   143,   145,   147,   149,   151
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      41,     0,    -1,    42,    -1,    42,    43,    -1,    43,    -1,
      44,    34,    -1,    45,    34,    -1,    46,    34,    -1,    47,
      34,    -1,    48,    -1,    49,    -1,    50,    -1,    51,    14,
      -1,    51,    14,    35,    52,    -1,    14,    35,    52,    -1,
      13,    36,    52,    37,    -1,    12,    52,    -1,     8,    36,
      52,    37,    43,    -1,     8,    36,    52,    37,    43,     9,
      43,    -1,    10,    36,    52,    37,    43,    -1,    38,    42,
      39,    -1,     3,    -1,     4,    -1,     5,    -1,     6,    -1,
       7,    -1,    52,    27,    52,    -1,    52,    28,    52,    -1,
      52,    29,    52,    -1,    52,    30,    52,    -1,    52,    31,
      52,    -1,    52,    19,    52,    -1,    52,    20,    52,    -1,
      52,    25,    52,    -1,    52,    26,    52,    -1,    52,    21,
      52,    -1,    52,    22,    52,    -1,    52,    23,    52,    -1,
      52,    24,    52,    -1,    32,    52,    -1,    36,    52,    37,
      -1,    14,    -1,    53,    -1,    15,    -1,    16,    -1,    17,
      -1,    18,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   165,   165,   174,   181,   190,   194,   198,   202,   206,
     210,   214,   221,   228,   239,   249,   258,   267,   274,   285,
     295,   304,   308,   312,   316,   320,   327,   334,   341,   348,
     355,   362,   369,   376,   383,   390,   397,   404,   411,   418,
     424,   428,   432,   439,   443,   447,   451
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOKEN_INT", "TOKEN_FLOAT", "TOKEN_CHAR",
  "TOKEN_STRING", "TOKEN_VOID", "TOKEN_IF", "TOKEN_ELSE", "TOKEN_WHILE",
  "TOKEN_FOR", "TOKEN_RETURN", "TOKEN_PRINT", "TOKEN_IDENTIFIER",
  "TOKEN_INTEGER", "TOKEN_FLOAT_LITERAL", "TOKEN_STRING_LITERAL",
  "TOKEN_CHAR_LITERAL", "TOKEN_EQ", "TOKEN_NE", "TOKEN_LE", "TOKEN_GE",
  "TOKEN_AND", "TOKEN_OR", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'",
  "'!'", "LOWER_THAN_ELSE", "';'", "'='", "'('", "')'", "'{'", "'}'",
  "$accept", "program", "statement_list", "statement", "declaration",
  "assignment", "print_statement", "return_statement", "if_statement",
  "while_statement", "block", "type", "expression", "literal", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,    60,    62,    43,    45,    42,
      47,    37,    33,   280,    59,    61,    40,    41,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    40,    41,    42,    42,    43,    43,    43,    43,    43,
      43,    43,    44,    44,    45,    46,    47,    48,    48,    49,
      50,    51,    51,    51,    51,    51,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    53,    53,    53,    53
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     2,     2,     2,     2,     1,
       1,     1,     2,     4,     3,     4,     2,     5,     7,     5,
       3,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       3,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    21,    22,    23,    24,    25,     0,     0,     0,     0,
       0,     0,     0,     2,     4,     0,     0,     0,     0,     9,
      10,    11,     0,     0,     0,    41,    43,    44,    45,    46,
       0,     0,    16,    42,     0,     0,     0,     1,     3,     5,
       6,     7,     8,    12,     0,     0,    39,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    14,    20,     0,     0,     0,    40,    31,    32,
      35,    36,    37,    38,    33,    34,    26,    27,    28,    29,
      30,    15,    13,    17,    19,     0,    18
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    32,    33
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -27
static const yytype_int16 yypact[] =
{
      63,   -27,   -27,   -27,   -27,   -27,   -26,   -25,   -11,   -16,
     -21,    63,    22,    63,   -27,   -10,     7,    16,    17,   -27,
     -27,   -27,    46,   -11,   -11,   -27,   -27,   -27,   -27,   -27,
     -11,   -11,   159,   -27,   -11,   -11,    51,   -27,   -27,   -27,
     -27,   -27,   -27,    27,    83,   102,   -27,   121,   -11,   -11,
     -11,   -11,   -11,   -11,   -11,   -11,   -11,   -11,   -11,   -11,
     -11,   140,   159,   -27,   -11,    63,    63,   -27,    18,    18,
     -12,   -12,    66,   172,   -12,   -12,    49,    49,   -27,   -27,
     -27,   -27,   159,    65,   -27,    63,   -27
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -27,   -27,    70,   -13,   -27,   -27,   -27,   -27,   -27,   -27,
     -27,   -27,   -22,   -27
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      38,    44,    45,    25,    26,    27,    28,    29,    46,    47,
      23,    24,    61,    62,    35,    56,    57,    58,    59,    60,
      34,    30,    37,    38,    39,    31,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    50,
      51,    40,    82,    54,    55,    56,    57,    58,    59,    60,
      41,    42,    83,    84,     1,     2,     3,     4,     5,     6,
      43,     7,    64,     8,     9,    10,     1,     2,     3,     4,
       5,     6,    86,     7,    85,     8,     9,    10,    58,    59,
      60,    36,     0,     0,     0,    48,    49,    50,    51,    11,
      63,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,    11,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,     0,     0,     0,     0,
      65,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,     0,     0,     0,     0,     0,    66,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,     0,     0,     0,     0,     0,    67,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,     0,     0,     0,     0,     0,    81,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    48,    49,    50,    51,    52,     0,    54,    55,    56,
      57,    58,    59,    60
};

static const yytype_int8 yycheck[] =
{
      13,    23,    24,    14,    15,    16,    17,    18,    30,    31,
      36,    36,    34,    35,    35,    27,    28,    29,    30,    31,
      36,    32,     0,    36,    34,    36,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    21,
      22,    34,    64,    25,    26,    27,    28,    29,    30,    31,
      34,    34,    65,    66,     3,     4,     5,     6,     7,     8,
      14,    10,    35,    12,    13,    14,     3,     4,     5,     6,
       7,     8,    85,    10,     9,    12,    13,    14,    29,    30,
      31,    11,    -1,    -1,    -1,    19,    20,    21,    22,    38,
      39,    25,    26,    27,    28,    29,    30,    31,    -1,    -1,
      -1,    38,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,
      37,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    37,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    -1,    -1,    -1,    -1,    -1,    37,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    -1,    -1,    -1,    -1,    -1,    37,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    19,    20,    21,    22,    23,    -1,    25,    26,    27,
      28,    29,    30,    31
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,    10,    12,    13,
      14,    38,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    36,    36,    14,    15,    16,    17,    18,
      32,    36,    52,    53,    36,    35,    42,     0,    43,    34,
      34,    34,    34,    14,    52,    52,    52,    52,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    52,    52,    39,    35,    37,    37,    37,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    37,    52,    43,    43,     9,    43
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 166 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (1)].node)));
          parse_tree = node("Program", std::move(children));
      ;}
    break;

  case 3:
#line 175 "parser.y"
    {
          auto children = std::move((yyvsp[(1) - (2)].node)->children);
          delete (yyvsp[(1) - (2)].node);
          append_child(children, std::unique_ptr<Node>((yyvsp[(2) - (2)].node)));
          (yyval.node) = node("Statements", std::move(children)).release();
      ;}
    break;

  case 4:
#line 182 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (1)].node)));
          (yyval.node) = node("Statements", std::move(children)).release();
      ;}
    break;

  case 5:
#line 191 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (2)].node);
      ;}
    break;

  case 6:
#line 195 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (2)].node);
      ;}
    break;

  case 7:
#line 199 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (2)].node);
      ;}
    break;

  case 8:
#line 203 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (2)].node);
      ;}
    break;

  case 9:
#line 207 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (1)].node);
      ;}
    break;

  case 10:
#line 211 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (1)].node);
      ;}
    break;

  case 11:
#line 215 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (1)].node);
      ;}
    break;

  case 12:
#line 222 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (2)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(2) - (2)].node)));
          (yyval.node) = node("Declaration", std::move(children)).release();
      ;}
    break;

  case 13:
#line 229 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (4)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(2) - (4)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(4) - (4)].node)));
          (yyval.node) = node("Declaration", std::move(children)).release();
      ;}
    break;

  case 14:
#line 240 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Assignment", std::move(children)).release();
      ;}
    break;

  case 15:
#line 250 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (4)].node)));
          (yyval.node) = node("Print", std::move(children)).release();
      ;}
    break;

  case 16:
#line 259 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(2) - (2)].node)));
          (yyval.node) = node("Return", std::move(children)).release();
      ;}
    break;

  case 17:
#line 268 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (5)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(5) - (5)].node)));
          (yyval.node) = node("If", std::move(children)).release();
      ;}
    break;

  case 18:
#line 275 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (7)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(5) - (7)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(7) - (7)].node)));
          (yyval.node) = node("IfElse", std::move(children)).release();
      ;}
    break;

  case 19:
#line 286 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (5)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(5) - (5)].node)));
          (yyval.node) = node("While", std::move(children)).release();
      ;}
    break;

  case 20:
#line 296 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(2) - (3)].node)));
          (yyval.node) = node("Block", std::move(children)).release();
      ;}
    break;

  case 21:
#line 305 "parser.y"
    {
          (yyval.node) = node("Type: int").release();
      ;}
    break;

  case 22:
#line 309 "parser.y"
    {
          (yyval.node) = node("Type: float").release();
      ;}
    break;

  case 23:
#line 313 "parser.y"
    {
          (yyval.node) = node("Type: char").release();
      ;}
    break;

  case 24:
#line 317 "parser.y"
    {
          (yyval.node) = node("Type: string").release();
      ;}
    break;

  case 25:
#line 321 "parser.y"
    {
          (yyval.node) = node("Type: void").release();
      ;}
    break;

  case 26:
#line 328 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: +", std::move(children)).release();
      ;}
    break;

  case 27:
#line 335 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: -", std::move(children)).release();
      ;}
    break;

  case 28:
#line 342 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: *", std::move(children)).release();
      ;}
    break;

  case 29:
#line 349 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: /", std::move(children)).release();
      ;}
    break;

  case 30:
#line 356 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: %", std::move(children)).release();
      ;}
    break;

  case 31:
#line 363 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: ==", std::move(children)).release();
      ;}
    break;

  case 32:
#line 370 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: !=", std::move(children)).release();
      ;}
    break;

  case 33:
#line 377 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: <", std::move(children)).release();
      ;}
    break;

  case 34:
#line 384 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: >", std::move(children)).release();
      ;}
    break;

  case 35:
#line 391 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: <=", std::move(children)).release();
      ;}
    break;

  case 36:
#line 398 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: >=", std::move(children)).release();
      ;}
    break;

  case 37:
#line 405 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: &&", std::move(children)).release();
      ;}
    break;

  case 38:
#line 412 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(1) - (3)].node)));
          append_child(children, std::unique_ptr<Node>((yyvsp[(3) - (3)].node)));
          (yyval.node) = node("Operator: ||", std::move(children)).release();
      ;}
    break;

  case 39:
#line 419 "parser.y"
    {
          std::vector<std::unique_ptr<Node>> children;
          append_child(children, std::unique_ptr<Node>((yyvsp[(2) - (2)].node)));
          (yyval.node) = node("Operator: !", std::move(children)).release();
      ;}
    break;

  case 40:
#line 425 "parser.y"
    {
          (yyval.node) = (yyvsp[(2) - (3)].node);
      ;}
    break;

  case 41:
#line 429 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (1)].node);
      ;}
    break;

  case 42:
#line 433 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (1)].node);
      ;}
    break;

  case 43:
#line 440 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (1)].node);
      ;}
    break;

  case 44:
#line 444 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (1)].node);
      ;}
    break;

  case 45:
#line 448 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (1)].node);
      ;}
    break;

  case 46:
#line 452 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (1)].node);
      ;}
    break;


/* Line 1267 of yacc.c.  */
#line 1981 "build/parser.tab.cc"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 457 "parser.y"


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

