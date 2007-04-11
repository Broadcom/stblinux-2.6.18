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
     INT = 258,
     NAME = 259,
     LNAME = 260,
     OREQ = 261,
     ANDEQ = 262,
     RSHIFTEQ = 263,
     LSHIFTEQ = 264,
     DIVEQ = 265,
     MULTEQ = 266,
     MINUSEQ = 267,
     PLUSEQ = 268,
     OROR = 269,
     ANDAND = 270,
     NE = 271,
     EQ = 272,
     GE = 273,
     LE = 274,
     RSHIFT = 275,
     LSHIFT = 276,
     UNARY = 277,
     END = 278,
     ALIGN_K = 279,
     BLOCK = 280,
     BIND = 281,
     QUAD = 282,
     SQUAD = 283,
     LONG = 284,
     SHORT = 285,
     BYTE = 286,
     SECTIONS = 287,
     PHDRS = 288,
     DATA_SEGMENT_ALIGN = 289,
     DATA_SEGMENT_RELRO_END = 290,
     DATA_SEGMENT_END = 291,
     SORT_BY_NAME = 292,
     SORT_BY_ALIGNMENT = 293,
     SIZEOF_HEADERS = 294,
     OUTPUT_FORMAT = 295,
     FORCE_COMMON_ALLOCATION = 296,
     OUTPUT_ARCH = 297,
     INHIBIT_COMMON_ALLOCATION = 298,
     SEGMENT_START = 299,
     INCLUDE = 300,
     MEMORY = 301,
     NOLOAD = 302,
     DSECT = 303,
     COPY = 304,
     INFO = 305,
     OVERLAY = 306,
     DEFINED = 307,
     TARGET_K = 308,
     SEARCH_DIR = 309,
     MAP = 310,
     ENTRY = 311,
     NEXT = 312,
     SIZEOF = 313,
     ADDR = 314,
     LOADADDR = 315,
     MAX_K = 316,
     MIN_K = 317,
     STARTUP = 318,
     HLL = 319,
     SYSLIB = 320,
     FLOAT = 321,
     NOFLOAT = 322,
     NOCROSSREFS = 323,
     ORIGIN = 324,
     FILL = 325,
     LENGTH = 326,
     CREATE_OBJECT_SYMBOLS = 327,
     INPUT = 328,
     GROUP = 329,
     OUTPUT = 330,
     CONSTRUCTORS = 331,
     ALIGNMOD = 332,
     AT = 333,
     SUBALIGN = 334,
     PROVIDE = 335,
     PROVIDE_HIDDEN = 336,
     AS_NEEDED = 337,
     CHIP = 338,
     LIST = 339,
     SECT = 340,
     ABSOLUTE = 341,
     LOAD = 342,
     NEWLINE = 343,
     ENDWORD = 344,
     ORDER = 345,
     NAMEWORD = 346,
     ASSERT_K = 347,
     FORMAT = 348,
     PUBLIC = 349,
     DEFSYMEND = 350,
     BASE = 351,
     ALIAS = 352,
     TRUNCATE = 353,
     REL = 354,
     INPUT_SCRIPT = 355,
     INPUT_MRI_SCRIPT = 356,
     INPUT_DEFSYM = 357,
     CASE = 358,
     EXTERN = 359,
     START = 360,
     VERS_TAG = 361,
     VERS_IDENTIFIER = 362,
     GLOBAL = 363,
     LOCAL = 364,
     VERSIONK = 365,
     INPUT_VERSION_SCRIPT = 366,
     KEEP = 367,
     ONLY_IF_RO = 368,
     ONLY_IF_RW = 369,
     SPECIAL = 370,
     EXCLUDE_FILE = 371,
     CONSTANT = 372,
     INPUT_DYNAMIC_LIST = 373
   };
#endif
#define INT 258
#define NAME 259
#define LNAME 260
#define OREQ 261
#define ANDEQ 262
#define RSHIFTEQ 263
#define LSHIFTEQ 264
#define DIVEQ 265
#define MULTEQ 266
#define MINUSEQ 267
#define PLUSEQ 268
#define OROR 269
#define ANDAND 270
#define NE 271
#define EQ 272
#define GE 273
#define LE 274
#define RSHIFT 275
#define LSHIFT 276
#define UNARY 277
#define END 278
#define ALIGN_K 279
#define BLOCK 280
#define BIND 281
#define QUAD 282
#define SQUAD 283
#define LONG 284
#define SHORT 285
#define BYTE 286
#define SECTIONS 287
#define PHDRS 288
#define DATA_SEGMENT_ALIGN 289
#define DATA_SEGMENT_RELRO_END 290
#define DATA_SEGMENT_END 291
#define SORT_BY_NAME 292
#define SORT_BY_ALIGNMENT 293
#define SIZEOF_HEADERS 294
#define OUTPUT_FORMAT 295
#define FORCE_COMMON_ALLOCATION 296
#define OUTPUT_ARCH 297
#define INHIBIT_COMMON_ALLOCATION 298
#define SEGMENT_START 299
#define INCLUDE 300
#define MEMORY 301
#define NOLOAD 302
#define DSECT 303
#define COPY 304
#define INFO 305
#define OVERLAY 306
#define DEFINED 307
#define TARGET_K 308
#define SEARCH_DIR 309
#define MAP 310
#define ENTRY 311
#define NEXT 312
#define SIZEOF 313
#define ADDR 314
#define LOADADDR 315
#define MAX_K 316
#define MIN_K 317
#define STARTUP 318
#define HLL 319
#define SYSLIB 320
#define FLOAT 321
#define NOFLOAT 322
#define NOCROSSREFS 323
#define ORIGIN 324
#define FILL 325
#define LENGTH 326
#define CREATE_OBJECT_SYMBOLS 327
#define INPUT 328
#define GROUP 329
#define OUTPUT 330
#define CONSTRUCTORS 331
#define ALIGNMOD 332
#define AT 333
#define SUBALIGN 334
#define PROVIDE 335
#define PROVIDE_HIDDEN 336
#define AS_NEEDED 337
#define CHIP 338
#define LIST 339
#define SECT 340
#define ABSOLUTE 341
#define LOAD 342
#define NEWLINE 343
#define ENDWORD 344
#define ORDER 345
#define NAMEWORD 346
#define ASSERT_K 347
#define FORMAT 348
#define PUBLIC 349
#define DEFSYMEND 350
#define BASE 351
#define ALIAS 352
#define TRUNCATE 353
#define REL 354
#define INPUT_SCRIPT 355
#define INPUT_MRI_SCRIPT 356
#define INPUT_DEFSYM 357
#define CASE 358
#define EXTERN 359
#define START 360
#define VERS_TAG 361
#define VERS_IDENTIFIER 362
#define GLOBAL 363
#define LOCAL 364
#define VERSIONK 365
#define INPUT_VERSION_SCRIPT 366
#define KEEP 367
#define ONLY_IF_RO 368
#define ONLY_IF_RW 369
#define SPECIAL 370
#define EXCLUDE_FILE 371
#define CONSTANT 372
#define INPUT_DYNAMIC_LIST 373




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 65 "ldgram.y"
typedef union YYSTYPE {
  bfd_vma integer;
  struct big_int
    {
      bfd_vma integer;
      char *str;
    } bigint;
  fill_type *fill;
  char *name;
  const char *cname;
  struct wildcard_spec wildcard;
  struct wildcard_list *wildcard_list;
  struct name_list *name_list;
  int token;
  union etree_union *etree;
  struct phdr_info
    {
      bfd_boolean filehdr;
      bfd_boolean phdrs;
      union etree_union *at;
      union etree_union *flags;
    } phdr;
  struct lang_nocrossref *nocrossref;
  struct lang_output_section_phdr_list *section_phdr;
  struct bfd_elf_version_deps *deflist;
  struct bfd_elf_version_expr *versyms;
  struct bfd_elf_version_tree *versnode;
} YYSTYPE;
/* Line 1275 of yacc.c.  */
#line 302 "ldgram.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



