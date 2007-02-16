/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IF = 258,
     THEN = 259,
     ELSE = 260,
     ELIF = 261,
     FI = 262,
     CASE = 263,
     ESAC = 264,
     FOR = 265,
     SELECT = 266,
     WHILE = 267,
     UNTIL = 268,
     DO = 269,
     DONE = 270,
     FUNCTION = 271,
     COND_START = 272,
     COND_END = 273,
     COND_ERROR = 274,
     IN = 275,
     BANG = 276,
     TIME = 277,
     TIMEOPT = 278,
     WORD = 279,
     ASSIGNMENT_WORD = 280,
     NUMBER = 281,
     ARITH_CMD = 282,
     ARITH_FOR_EXPRS = 283,
     COND_CMD = 284,
     AND_AND = 285,
     OR_OR = 286,
     GREATER_GREATER = 287,
     LESS_LESS = 288,
     LESS_AND = 289,
     GREATER_AND = 290,
     SEMI_SEMI = 291,
     LESS_LESS_MINUS = 292,
     AND_GREATER = 293,
     LESS_GREATER = 294,
     GREATER_BAR = 295,
     yacc_EOF = 296
   };
#endif
#define IF 258
#define THEN 259
#define ELSE 260
#define ELIF 261
#define FI 262
#define CASE 263
#define ESAC 264
#define FOR 265
#define SELECT 266
#define WHILE 267
#define UNTIL 268
#define DO 269
#define DONE 270
#define FUNCTION 271
#define COND_START 272
#define COND_END 273
#define COND_ERROR 274
#define IN 275
#define BANG 276
#define TIME 277
#define TIMEOPT 278
#define WORD 279
#define ASSIGNMENT_WORD 280
#define NUMBER 281
#define ARITH_CMD 282
#define ARITH_FOR_EXPRS 283
#define COND_CMD 284
#define AND_AND 285
#define OR_OR 286
#define GREATER_GREATER 287
#define LESS_LESS 288
#define LESS_AND 289
#define GREATER_AND 290
#define SEMI_SEMI 291
#define LESS_LESS_MINUS 292
#define AND_GREATER 293
#define LESS_GREATER 294
#define GREATER_BAR 295
#define yacc_EOF 296




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 190 "./parse.y"
typedef union YYSTYPE {
  WORD_DESC *word;		/* the word that we read. */
  int number;			/* the number that we read. */
  WORD_LIST *word_list;
  COMMAND *command;
  REDIRECT *redirect;
  ELEMENT element;
  PATTERN_LIST *pattern;
} YYSTYPE;
/* Line 1275 of yacc.c.  */
#line 129 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



