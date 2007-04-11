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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

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




/* Copy the first part of user declarations.  */
#line 22 "ldgram.y"

/*

 */

#define DONTDECLARE_MALLOC

#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"
#include "ld.h"
#include "ldexp.h"
#include "ldver.h"
#include "ldlang.h"
#include "ldfile.h"
#include "ldemul.h"
#include "ldmisc.h"
#include "ldmain.h"
#include "mri.h"
#include "ldctor.h"
#include "ldlex.h"

#ifndef YYDEBUG
#define YYDEBUG 1
#endif

static enum section_type sectype;
static lang_memory_region_type *region;

FILE *saved_script_handle = NULL;
bfd_boolean force_make_executable = FALSE;

bfd_boolean ldgram_in_script = FALSE;
bfd_boolean ldgram_had_equals = FALSE;
bfd_boolean ldgram_had_keep = FALSE;
char *ldgram_vers_current_lang = NULL;

#define ERROR_NAME_MAX 20
static char *error_names[ERROR_NAME_MAX];
static int error_index;
#define PUSH_ERROR(x) if (error_index < ERROR_NAME_MAX) error_names[error_index] = x; error_index++;
#define POP_ERROR()   error_index--;


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
/* Line 191 of yacc.c.  */
#line 385 "ldgram.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 397 "ldgram.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
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
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  17
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1730

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  142
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  119
/* YYNRULES -- Number of rules. */
#define YYNRULES  341
/* YYNRULES -- Number of states. */
#define YYNSTATES  723

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   373

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   140,     2,     2,     2,    34,    21,     2,
      37,   137,    32,    30,   135,    31,     2,    33,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    16,   136,
      24,     6,    25,    15,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   138,     2,   139,    20,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    53,    19,    54,   141,     2,     2,     2,
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
       5,     7,     8,     9,    10,    11,    12,    13,    14,    17,
      18,    22,    23,    26,    27,    28,    29,    35,    36,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     6,     9,    12,    15,    18,    20,    21,
      26,    27,    30,    34,    35,    38,    43,    45,    47,    50,
      52,    57,    62,    66,    69,    74,    78,    83,    88,    93,
      98,   103,   106,   109,   112,   117,   122,   125,   128,   131,
     134,   135,   141,   144,   145,   149,   152,   153,   155,   159,
     161,   165,   166,   168,   172,   174,   177,   181,   182,   185,
     188,   189,   191,   193,   195,   197,   199,   201,   203,   205,
     207,   209,   214,   219,   224,   229,   238,   243,   245,   247,
     252,   253,   259,   264,   265,   271,   276,   281,   283,   287,
     290,   292,   296,   299,   300,   306,   307,   315,   316,   323,
     328,   331,   334,   335,   340,   343,   344,   352,   354,   356,
     358,   360,   366,   371,   376,   384,   392,   400,   408,   417,
     420,   422,   426,   428,   430,   434,   439,   441,   442,   448,
     451,   453,   455,   457,   462,   464,   469,   474,   477,   479,
     480,   482,   484,   486,   488,   490,   492,   494,   497,   498,
     500,   502,   504,   506,   508,   510,   512,   514,   516,   518,
     522,   526,   533,   540,   542,   543,   549,   552,   556,   557,
     558,   566,   570,   574,   575,   579,   581,   584,   586,   589,
     594,   599,   603,   607,   609,   614,   618,   619,   621,   623,
     624,   627,   631,   632,   635,   638,   642,   647,   650,   653,
     656,   660,   664,   668,   672,   676,   680,   684,   688,   692,
     696,   700,   704,   708,   712,   716,   720,   726,   730,   734,
     739,   741,   743,   748,   753,   758,   763,   768,   773,   780,
     787,   794,   799,   806,   811,   813,   820,   827,   834,   839,
     844,   848,   849,   854,   855,   860,   861,   866,   867,   869,
     871,   873,   874,   875,   876,   877,   878,   879,   899,   900,
     901,   902,   903,   904,   923,   924,   925,   933,   935,   937,
     939,   941,   943,   947,   948,   951,   955,   958,   965,   976,
     979,   981,   982,   984,   987,   988,   989,   993,   994,   995,
     996,   997,  1009,  1014,  1015,  1018,  1019,  1020,  1027,  1029,
    1030,  1034,  1040,  1041,  1045,  1046,  1049,  1051,  1054,  1059,
    1062,  1063,  1066,  1067,  1073,  1075,  1078,  1083,  1089,  1096,
    1098,  1101,  1102,  1105,  1110,  1115,  1124,  1126,  1128,  1132,
    1136,  1137,  1147,  1148,  1156,  1158,  1162,  1164,  1168,  1170,
    1174,  1175
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     143,     0,    -1,   116,   157,    -1,   117,   147,    -1,   127,
     249,    -1,   134,   244,    -1,   118,   145,    -1,     4,    -1,
      -1,   146,     4,     6,   206,    -1,    -1,   148,   149,    -1,
     149,   150,   104,    -1,    -1,    99,   206,    -1,    99,   206,
     135,   206,    -1,     4,    -1,   100,    -1,   106,   152,    -1,
     105,    -1,   110,     4,     6,   206,    -1,   110,     4,   135,
     206,    -1,   110,     4,   206,    -1,   109,     4,    -1,   101,
       4,   135,   206,    -1,   101,     4,   206,    -1,   101,     4,
       6,   206,    -1,    38,     4,     6,   206,    -1,    38,     4,
     135,   206,    -1,    93,     4,     6,   206,    -1,    93,     4,
     135,   206,    -1,   102,   154,    -1,   103,   153,    -1,   107,
       4,    -1,   113,     4,   135,     4,    -1,   113,     4,   135,
       3,    -1,   112,   206,    -1,   114,     3,    -1,   119,   155,
      -1,   120,   156,    -1,    -1,    61,   144,   151,   149,    36,
      -1,   121,     4,    -1,    -1,   152,   135,     4,    -1,   152,
       4,    -1,    -1,     4,    -1,   153,   135,     4,    -1,     4,
      -1,   154,   135,     4,    -1,    -1,     4,    -1,   155,   135,
       4,    -1,     4,    -1,   156,     4,    -1,   156,   135,     4,
      -1,    -1,   158,   159,    -1,   159,   160,    -1,    -1,   188,
      -1,   167,    -1,   236,    -1,   197,    -1,   198,    -1,   200,
      -1,   202,    -1,   169,    -1,   251,    -1,   136,    -1,    69,
      37,     4,   137,    -1,    70,    37,   144,   137,    -1,    91,
      37,   144,   137,    -1,    56,    37,     4,   137,    -1,    56,
      37,     4,   135,     4,   135,     4,   137,    -1,    58,    37,
       4,   137,    -1,    57,    -1,    59,    -1,    89,    37,   163,
     137,    -1,    -1,    90,   161,    37,   163,   137,    -1,    71,
      37,   144,   137,    -1,    -1,    61,   144,   162,   159,    36,
      -1,    84,    37,   203,   137,    -1,   120,    37,   156,   137,
      -1,     4,    -1,   163,   135,     4,    -1,   163,     4,    -1,
       5,    -1,   163,   135,     5,    -1,   163,     5,    -1,    -1,
      98,    37,   164,   163,   137,    -1,    -1,   163,   135,    98,
      37,   165,   163,   137,    -1,    -1,   163,    98,    37,   166,
     163,   137,    -1,    46,    53,   168,    54,    -1,   168,   212,
      -1,   168,   169,    -1,    -1,    72,    37,     4,   137,    -1,
     186,   185,    -1,    -1,   108,   170,    37,   206,   135,     4,
     137,    -1,     4,    -1,    32,    -1,    15,    -1,   171,    -1,
     132,    37,   173,   137,   171,    -1,    51,    37,   171,   137,
      -1,    52,    37,   171,   137,    -1,    51,    37,    52,    37,
     171,   137,   137,    -1,    51,    37,    51,    37,   171,   137,
     137,    -1,    52,    37,    51,    37,   171,   137,   137,    -1,
      52,    37,    52,    37,   171,   137,   137,    -1,    51,    37,
     132,    37,   173,   137,   171,   137,    -1,   173,   171,    -1,
     171,    -1,   174,   187,   172,    -1,   172,    -1,     4,    -1,
     138,   174,   139,    -1,   172,    37,   174,   137,    -1,   175,
      -1,    -1,   128,    37,   177,   175,   137,    -1,   186,   185,
      -1,    88,    -1,   136,    -1,    92,    -1,    51,    37,    92,
     137,    -1,   176,    -1,   181,    37,   204,   137,    -1,    86,
      37,   182,   137,    -1,   179,   178,    -1,   178,    -1,    -1,
     179,    -1,    41,    -1,    42,    -1,    43,    -1,    44,    -1,
      45,    -1,   204,    -1,     6,   182,    -1,    -1,    14,    -1,
      13,    -1,    12,    -1,    11,    -1,    10,    -1,     9,    -1,
       8,    -1,     7,    -1,   136,    -1,   135,    -1,     4,     6,
     204,    -1,     4,   184,   204,    -1,    96,    37,     4,     6,
     204,   137,    -1,    97,    37,     4,     6,   204,   137,    -1,
     135,    -1,    -1,    62,    53,   190,   189,    54,    -1,   189,
     190,    -1,   189,   135,   190,    -1,    -1,    -1,     4,   191,
     194,    16,   192,   187,   193,    -1,    85,     6,   204,    -1,
      87,     6,   204,    -1,    -1,    37,   195,   137,    -1,   196,
      -1,   195,   196,    -1,     4,    -1,   140,     4,    -1,    79,
      37,   144,   137,    -1,    80,    37,   199,   137,    -1,    80,
      37,   137,    -1,   199,   187,   144,    -1,   144,    -1,    81,
      37,   201,   137,    -1,   201,   187,   144,    -1,    -1,    82,
      -1,    83,    -1,    -1,     4,   203,    -1,     4,   135,   203,
      -1,    -1,   205,   206,    -1,    31,   206,    -1,    37,   206,
     137,    -1,    73,    37,   206,   137,    -1,   140,   206,    -1,
      30,   206,    -1,   141,   206,    -1,   206,    32,   206,    -1,
     206,    33,   206,    -1,   206,    34,   206,    -1,   206,    30,
     206,    -1,   206,    31,   206,    -1,   206,    29,   206,    -1,
     206,    28,   206,    -1,   206,    23,   206,    -1,   206,    22,
     206,    -1,   206,    27,   206,    -1,   206,    26,   206,    -1,
     206,    24,   206,    -1,   206,    25,   206,    -1,   206,    21,
     206,    -1,   206,    20,   206,    -1,   206,    19,   206,    -1,
     206,    15,   206,    16,   206,    -1,   206,    18,   206,    -1,
     206,    17,   206,    -1,    68,    37,     4,   137,    -1,     3,
      -1,    55,    -1,    74,    37,     4,   137,    -1,    75,    37,
       4,   137,    -1,    76,    37,     4,   137,    -1,   133,    37,
       4,   137,    -1,   102,    37,   206,   137,    -1,    38,    37,
     206,   137,    -1,    38,    37,   206,   135,   206,   137,    -1,
      48,    37,   206,   135,   206,   137,    -1,    49,    37,   206,
     135,   206,   137,    -1,    50,    37,   206,   137,    -1,    60,
      37,     4,   135,   206,   137,    -1,    39,    37,   206,   137,
      -1,     4,    -1,    77,    37,   206,   135,   206,   137,    -1,
      78,    37,   206,   135,   206,   137,    -1,   108,    37,   206,
     135,     4,   137,    -1,    85,    37,     4,   137,    -1,    87,
      37,     4,   137,    -1,    94,    25,     4,    -1,    -1,    94,
      37,   206,   137,    -1,    -1,    38,    37,   206,   137,    -1,
      -1,    95,    37,   206,   137,    -1,    -1,   129,    -1,   130,
      -1,   131,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,
     213,   227,   208,   209,   210,   214,   211,    53,   215,   180,
      54,   216,   230,   207,   231,   183,   217,   187,    -1,    -1,
      -1,    -1,    -1,    -1,    67,   218,   228,   229,   208,   210,
     219,    53,   220,   232,    54,   221,   230,   207,   231,   183,
     222,   187,    -1,    -1,    -1,    90,   223,   227,   224,    53,
     168,    54,    -1,    63,    -1,    64,    -1,    65,    -1,    66,
      -1,    67,    -1,    37,   225,   137,    -1,    -1,    37,   137,
      -1,   206,   226,    16,    -1,   226,    16,    -1,    40,    37,
     206,   137,   226,    16,    -1,    40,    37,   206,   137,    39,
      37,   206,   137,   226,    16,    -1,   206,    16,    -1,    16,
      -1,    -1,    84,    -1,    25,     4,    -1,    -1,    -1,   231,
      16,     4,    -1,    -1,    -1,    -1,    -1,   232,     4,   233,
      53,   180,    54,   234,   231,   183,   235,   187,    -1,    47,
      53,   237,    54,    -1,    -1,   237,   238,    -1,    -1,    -1,
       4,   239,   241,   242,   240,   136,    -1,   206,    -1,    -1,
       4,   243,   242,    -1,    94,    37,   206,   137,   242,    -1,
      -1,    37,   206,   137,    -1,    -1,   245,   246,    -1,   247,
      -1,   246,   247,    -1,    53,   248,    54,   136,    -1,   257,
     136,    -1,    -1,   250,   253,    -1,    -1,   252,   126,    53,
     253,    54,    -1,   254,    -1,   253,   254,    -1,    53,   256,
      54,   136,    -1,   122,    53,   256,    54,   136,    -1,   122,
      53,   256,    54,   255,   136,    -1,   122,    -1,   255,   122,
      -1,    -1,   257,   136,    -1,   124,    16,   257,   136,    -1,
     125,    16,   257,   136,    -1,   124,    16,   257,   136,   125,
      16,   257,   136,    -1,   123,    -1,     4,    -1,   257,   136,
     123,    -1,   257,   136,     4,    -1,    -1,   257,   136,   120,
       4,    53,   258,   257,   260,    54,    -1,    -1,   120,     4,
      53,   259,   257,   260,    54,    -1,   124,    -1,   257,   136,
     124,    -1,   125,    -1,   257,   136,   125,    -1,   120,    -1,
     257,   136,   120,    -1,    -1,   136,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   163,   163,   164,   165,   166,   167,   171,   175,   175,
     185,   185,   198,   199,   203,   204,   205,   208,   211,   212,
     213,   215,   217,   219,   221,   223,   225,   227,   229,   231,
     233,   235,   236,   237,   239,   241,   243,   245,   247,   248,
     250,   249,   253,   255,   259,   260,   261,   265,   267,   271,
     273,   278,   279,   280,   284,   286,   288,   293,   293,   304,
     305,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   323,   325,   327,   330,   332,   334,   336,   338,
     340,   339,   343,   346,   345,   349,   353,   357,   360,   363,
     366,   369,   372,   376,   375,   380,   379,   384,   383,   390,
     394,   395,   396,   400,   402,   403,   403,   411,   415,   419,
     426,   432,   438,   444,   450,   456,   462,   468,   474,   483,
     492,   503,   512,   523,   531,   535,   542,   544,   543,   550,
     551,   555,   556,   561,   566,   567,   572,   579,   580,   583,
     585,   589,   591,   593,   595,   597,   602,   609,   611,   615,
     617,   619,   621,   623,   625,   627,   629,   634,   634,   639,
     643,   651,   655,   663,   663,   667,   671,   672,   673,   678,
     677,   685,   693,   701,   702,   706,   707,   711,   713,   718,
     723,   724,   729,   731,   737,   739,   741,   745,   747,   753,
     756,   765,   776,   776,   782,   784,   786,   788,   790,   792,
     795,   797,   799,   801,   803,   805,   807,   809,   811,   813,
     815,   817,   819,   821,   823,   825,   827,   829,   831,   833,
     835,   837,   840,   842,   844,   846,   848,   850,   852,   854,
     856,   858,   860,   869,   871,   873,   875,   877,   879,   881,
     887,   888,   892,   893,   897,   898,   902,   903,   907,   908,
     909,   910,   913,   917,   920,   926,   928,   913,   935,   937,
     939,   944,   946,   934,   956,   958,   956,   966,   967,   968,
     969,   970,   974,   975,   976,   980,   981,   986,   987,   992,
     993,   998,   999,  1004,  1006,  1011,  1014,  1027,  1031,  1036,
    1038,  1029,  1046,  1049,  1051,  1055,  1056,  1055,  1065,  1110,
    1113,  1125,  1134,  1137,  1144,  1144,  1156,  1157,  1161,  1165,
    1174,  1174,  1188,  1188,  1198,  1199,  1203,  1207,  1211,  1218,
    1222,  1230,  1233,  1237,  1241,  1245,  1252,  1256,  1260,  1264,
    1269,  1268,  1282,  1281,  1291,  1295,  1299,  1303,  1307,  1311,
    1317,  1319
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT", "NAME", "LNAME", "'='", "OREQ",
  "ANDEQ", "RSHIFTEQ", "LSHIFTEQ", "DIVEQ", "MULTEQ", "MINUSEQ", "PLUSEQ",
  "'?'", "':'", "OROR", "ANDAND", "'|'", "'^'", "'&'", "NE", "EQ", "'<'",
  "'>'", "GE", "LE", "RSHIFT", "LSHIFT", "'+'", "'-'", "'*'", "'/'", "'%'",
  "UNARY", "END", "'('", "ALIGN_K", "BLOCK", "BIND", "QUAD", "SQUAD",
  "LONG", "SHORT", "BYTE", "SECTIONS", "PHDRS", "DATA_SEGMENT_ALIGN",
  "DATA_SEGMENT_RELRO_END", "DATA_SEGMENT_END", "SORT_BY_NAME",
  "SORT_BY_ALIGNMENT", "'{'", "'}'", "SIZEOF_HEADERS", "OUTPUT_FORMAT",
  "FORCE_COMMON_ALLOCATION", "OUTPUT_ARCH", "INHIBIT_COMMON_ALLOCATION",
  "SEGMENT_START", "INCLUDE", "MEMORY", "NOLOAD", "DSECT", "COPY", "INFO",
  "OVERLAY", "DEFINED", "TARGET_K", "SEARCH_DIR", "MAP", "ENTRY", "NEXT",
  "SIZEOF", "ADDR", "LOADADDR", "MAX_K", "MIN_K", "STARTUP", "HLL",
  "SYSLIB", "FLOAT", "NOFLOAT", "NOCROSSREFS", "ORIGIN", "FILL", "LENGTH",
  "CREATE_OBJECT_SYMBOLS", "INPUT", "GROUP", "OUTPUT", "CONSTRUCTORS",
  "ALIGNMOD", "AT", "SUBALIGN", "PROVIDE", "PROVIDE_HIDDEN", "AS_NEEDED",
  "CHIP", "LIST", "SECT", "ABSOLUTE", "LOAD", "NEWLINE", "ENDWORD",
  "ORDER", "NAMEWORD", "ASSERT_K", "FORMAT", "PUBLIC", "DEFSYMEND", "BASE",
  "ALIAS", "TRUNCATE", "REL", "INPUT_SCRIPT", "INPUT_MRI_SCRIPT",
  "INPUT_DEFSYM", "CASE", "EXTERN", "START", "VERS_TAG", "VERS_IDENTIFIER",
  "GLOBAL", "LOCAL", "VERSIONK", "INPUT_VERSION_SCRIPT", "KEEP",
  "ONLY_IF_RO", "ONLY_IF_RW", "SPECIAL", "EXCLUDE_FILE", "CONSTANT",
  "INPUT_DYNAMIC_LIST", "','", "';'", "')'", "'['", "']'", "'!'", "'~'",
  "$accept", "file", "filename", "defsym_expr", "@1", "mri_script_file",
  "@2", "mri_script_lines", "mri_script_command", "@3", "ordernamelist",
  "mri_load_name_list", "mri_abs_name_list", "casesymlist",
  "extern_name_list", "script_file", "@4", "ifile_list", "ifile_p1", "@5",
  "@6", "input_list", "@7", "@8", "@9", "sections", "sec_or_group_p1",
  "statement_anywhere", "@10", "wildcard_name", "wildcard_spec",
  "exclude_name_list", "file_NAME_list", "input_section_spec_no_keep",
  "input_section_spec", "@11", "statement", "statement_list",
  "statement_list_opt", "length", "fill_exp", "fill_opt", "assign_op",
  "end", "assignment", "opt_comma", "memory", "memory_spec_list",
  "memory_spec", "@12", "origin_spec", "length_spec", "attributes_opt",
  "attributes_list", "attributes_string", "startup", "high_level_library",
  "high_level_library_NAME_list", "low_level_library",
  "low_level_library_NAME_list", "floating_point_support",
  "nocrossref_list", "mustbe_exp", "@13", "exp", "memspec_at_opt",
  "opt_at", "opt_align", "opt_subalign", "sect_constraint", "section",
  "@14", "@15", "@16", "@17", "@18", "@19", "@20", "@21", "@22", "@23",
  "@24", "@25", "type", "atype", "opt_exp_with_type",
  "opt_exp_without_type", "opt_nocrossrefs", "memspec_opt", "phdr_opt",
  "overlay_section", "@26", "@27", "@28", "phdrs", "phdr_list", "phdr",
  "@29", "@30", "phdr_type", "phdr_qualifiers", "phdr_val",
  "dynamic_list_file", "@31", "dynamic_list_nodes", "dynamic_list_node",
  "dynamic_list_tag", "version_script_file", "@32", "version", "@33",
  "vers_nodes", "vers_node", "verdep", "vers_tag", "vers_defns", "@34",
  "@35", "opt_semicolon", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,    61,   261,   262,   263,
     264,   265,   266,   267,   268,    63,    58,   269,   270,   124,
      94,    38,   271,   272,    60,    62,   273,   274,   275,   276,
      43,    45,    42,    47,    37,   277,   278,    40,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   123,   125,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
     349,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   362,   363,   364,   365,   366,   367,   368,
     369,   370,   371,   372,   373,    44,    59,    41,    91,    93,
      33,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   142,   143,   143,   143,   143,   143,   144,   146,   145,
     148,   147,   149,   149,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     151,   150,   150,   150,   152,   152,   152,   153,   153,   154,
     154,   155,   155,   155,   156,   156,   156,   158,   157,   159,
     159,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     161,   160,   160,   162,   160,   160,   160,   163,   163,   163,
     163,   163,   163,   164,   163,   165,   163,   166,   163,   167,
     168,   168,   168,   169,   169,   170,   169,   171,   171,   171,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   173,
     173,   174,   174,   175,   175,   175,   176,   177,   176,   178,
     178,   178,   178,   178,   178,   178,   178,   179,   179,   180,
     180,   181,   181,   181,   181,   181,   182,   183,   183,   184,
     184,   184,   184,   184,   184,   184,   184,   185,   185,   186,
     186,   186,   186,   187,   187,   188,   189,   189,   189,   191,
     190,   192,   193,   194,   194,   195,   195,   196,   196,   197,
     198,   198,   199,   199,   200,   201,   201,   202,   202,   203,
     203,   203,   205,   204,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     207,   207,   208,   208,   209,   209,   210,   210,   211,   211,
     211,   211,   213,   214,   215,   216,   217,   212,   218,   219,
     220,   221,   222,   212,   223,   224,   212,   225,   225,   225,
     225,   225,   226,   226,   226,   227,   227,   227,   227,   228,
     228,   229,   229,   230,   230,   231,   231,   232,   233,   234,
     235,   232,   236,   237,   237,   239,   240,   238,   241,   242,
     242,   242,   243,   243,   245,   244,   246,   246,   247,   248,
     250,   249,   252,   251,   253,   253,   254,   254,   254,   255,
     255,   256,   256,   256,   256,   256,   257,   257,   257,   257,
     258,   257,   259,   257,   257,   257,   257,   257,   257,   257,
     260,   260
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     2,     2,     2,     2,     1,     0,     4,
       0,     2,     3,     0,     2,     4,     1,     1,     2,     1,
       4,     4,     3,     2,     4,     3,     4,     4,     4,     4,
       4,     2,     2,     2,     4,     4,     2,     2,     2,     2,
       0,     5,     2,     0,     3,     2,     0,     1,     3,     1,
       3,     0,     1,     3,     1,     2,     3,     0,     2,     2,
       0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     4,     4,     4,     8,     4,     1,     1,     4,
       0,     5,     4,     0,     5,     4,     4,     1,     3,     2,
       1,     3,     2,     0,     5,     0,     7,     0,     6,     4,
       2,     2,     0,     4,     2,     0,     7,     1,     1,     1,
       1,     5,     4,     4,     7,     7,     7,     7,     8,     2,
       1,     3,     1,     1,     3,     4,     1,     0,     5,     2,
       1,     1,     1,     4,     1,     4,     4,     2,     1,     0,
       1,     1,     1,     1,     1,     1,     1,     2,     0,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     6,     6,     1,     0,     5,     2,     3,     0,     0,
       7,     3,     3,     0,     3,     1,     2,     1,     2,     4,
       4,     3,     3,     1,     4,     3,     0,     1,     1,     0,
       2,     3,     0,     2,     2,     3,     4,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     5,     3,     3,     4,
       1,     1,     4,     4,     4,     4,     4,     4,     6,     6,
       6,     4,     6,     4,     1,     6,     6,     6,     4,     4,
       3,     0,     4,     0,     4,     0,     4,     0,     1,     1,
       1,     0,     0,     0,     0,     0,     0,    19,     0,     0,
       0,     0,     0,    18,     0,     0,     7,     1,     1,     1,
       1,     1,     3,     0,     2,     3,     2,     6,    10,     2,
       1,     0,     1,     2,     0,     0,     3,     0,     0,     0,
       0,    11,     4,     0,     2,     0,     0,     6,     1,     0,
       3,     5,     0,     3,     0,     2,     1,     2,     4,     2,
       0,     2,     0,     5,     1,     2,     4,     5,     6,     1,
       2,     0,     2,     4,     4,     8,     1,     1,     3,     3,
       0,     9,     0,     7,     1,     3,     1,     3,     1,     3,
       0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       0,    57,    10,     8,   310,   304,     0,     2,    60,     3,
      13,     6,     0,     4,     0,     5,     0,     1,    58,    11,
       0,   321,     0,   311,   314,     0,   305,   306,     0,     0,
       0,     0,    77,     0,    78,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   187,   188,     0,     0,    80,     0,
       0,     0,   105,     0,    70,    59,    62,    68,     0,    61,
      64,    65,    66,    67,    63,    69,     0,    16,     0,     0,
       0,     0,    17,     0,     0,     0,    19,    46,     0,     0,
       0,     0,     0,     0,    51,     0,     0,     0,     0,   327,
     338,   326,   334,   336,     0,     0,   321,   315,   334,   336,
       0,     0,   307,   192,   156,   155,   154,   153,   152,   151,
     150,   149,   192,   102,   293,     0,     0,     7,    83,     0,
       0,     0,     0,     0,     0,     0,   186,   189,     0,     0,
       0,     0,     0,     0,     0,   158,   157,   104,     0,     0,
      40,     0,   220,   234,     0,     0,     0,     0,     0,     0,
       0,     0,   221,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
      49,    31,    47,    32,    18,    33,    23,     0,    36,     0,
      37,    52,    38,    54,    39,    42,    12,     9,     0,     0,
       0,     0,   322,     0,     0,   309,   159,     0,   160,     0,
       0,     0,     0,    60,   169,   168,     0,     0,     0,     0,
       0,   181,   183,   164,   164,   189,     0,    87,    90,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      13,     0,     0,   198,   194,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   197,   199,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    25,     0,
       0,    45,     0,     0,     0,    22,     0,     0,    55,     0,
     332,     0,     0,   316,   329,   339,   328,   335,   337,     0,
     308,   193,   252,    99,   258,   264,   101,   100,   295,   292,
     294,     0,    74,    76,   312,   173,     0,    71,    72,    82,
     103,   179,   163,   180,     0,   184,     0,   189,   190,    85,
      93,    89,    92,     0,     0,    79,     0,    73,   192,   192,
       0,    86,     0,    27,    28,    43,    29,    30,   195,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   218,   217,
     215,   214,   213,   208,   207,   211,   212,   210,   209,   206,
     205,   203,   204,   200,   201,   202,    15,    26,    24,    50,
      48,    44,    20,    21,    35,    34,    53,    56,     0,   323,
     324,     0,   319,   317,     0,   273,     0,   273,     0,     0,
      84,     0,     0,   165,     0,   166,   182,   185,   191,     0,
      97,    88,    91,     0,    81,     0,     0,     0,   313,    41,
       0,   227,   233,     0,     0,   231,     0,   219,   196,   222,
     223,   224,     0,     0,   238,   239,   226,     0,   225,     0,
     340,   337,   330,   320,   318,     0,     0,   273,     0,   243,
     280,     0,   281,   265,   298,   299,     0,   177,     0,     0,
     175,     0,   167,     0,     0,    95,   161,   162,     0,     0,
       0,     0,     0,     0,     0,     0,   216,   341,     0,     0,
       0,   267,   268,   269,   270,   271,   274,     0,     0,     0,
       0,   276,     0,   245,   279,   282,   243,     0,   302,     0,
     296,     0,   178,   174,   176,     0,   164,    94,     0,     0,
     106,   228,   229,   230,   232,   235,   236,   237,   333,     0,
     340,   272,     0,   275,     0,     0,   247,   247,   102,     0,
     299,     0,     0,    75,   192,     0,    98,     0,   325,     0,
     273,     0,     0,     0,   253,   259,     0,     0,   300,     0,
     297,   171,     0,   170,    96,   331,     0,     0,   242,     0,
       0,   251,     0,   266,   303,   299,   192,     0,   277,   244,
       0,   248,   249,   250,     0,   260,   301,   172,     0,   246,
     254,   287,   273,   139,     0,     0,   123,   109,   108,   141,
     142,   143,   144,   145,     0,     0,     0,   130,   132,     0,
       0,   131,     0,   110,     0,   126,   134,   138,   140,     0,
       0,     0,   288,   261,   278,     0,     0,   192,   127,     0,
     107,     0,   122,   164,     0,   137,   255,   192,   129,     0,
     284,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     146,     0,   120,     0,     0,   124,     0,   164,   284,     0,
     139,     0,   241,     0,     0,   133,     0,   112,     0,     0,
     113,   136,   107,     0,     0,   119,   121,   125,   241,   135,
       0,   283,     0,   285,     0,     0,     0,     0,     0,   128,
     111,   285,   289,     0,   148,     0,     0,     0,     0,     0,
     148,   285,   240,   192,     0,   262,   115,   114,     0,   116,
     117,   256,   148,   147,   286,   164,   118,   164,   290,   263,
     257,   164,   291
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     6,   118,    11,    12,     9,    10,    19,    87,   230,
     174,   173,   171,   182,   184,     7,     8,    18,    55,   129,
     203,   220,   419,   519,   474,    56,   199,    57,   133,   613,
     614,   653,   633,   615,   616,   651,   617,   618,   619,   620,
     649,   705,   112,   137,    58,   656,    59,   316,   205,   315,
     516,   563,   412,   469,   470,    60,    61,   213,    62,   214,
      63,   216,   650,   197,   235,   683,   503,   536,   554,   584,
     307,   405,   571,   593,   658,   717,   406,   572,   591,   640,
     715,   407,   507,   497,   458,   459,   462,   506,   662,   694,
     594,   639,   701,   721,    64,   200,   310,   408,   542,   465,
     510,   540,    15,    16,    26,    27,   100,    13,    14,    65,
      66,    23,    24,   404,    94,    95,   490,   398,   488
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -647
static const short yypact[] =
{
     181,  -647,  -647,  -647,  -647,  -647,    37,  -647,  -647,  -647,
    -647,  -647,    53,  -647,   -13,  -647,    48,  -647,   811,   453,
     102,     6,    72,   -13,  -647,   103,    48,  -647,   323,    75,
      95,   148,  -647,   149,  -647,   153,   136,   177,   188,   204,
     205,   209,   210,   228,  -647,  -647,   232,   235,  -647,   243,
     251,   252,  -647,   253,  -647,  -647,  -647,  -647,   -43,  -647,
    -647,  -647,  -647,  -647,  -647,  -647,    64,  -647,   214,   153,
     220,   690,  -647,   296,   297,   298,  -647,  -647,   305,   306,
     307,   690,   308,   310,   313,   314,   315,   217,   690,  -647,
     318,  -647,   309,   311,   260,   202,     6,  -647,  -647,  -647,
     286,   212,  -647,  -647,  -647,  -647,  -647,  -647,  -647,  -647,
    -647,  -647,  -647,  -647,  -647,   346,   348,  -647,  -647,   349,
     350,   153,   153,   353,   153,    10,  -647,   357,    38,   325,
     153,   359,   360,   328,   314,  -647,  -647,  -647,   330,    28,
    -647,    35,  -647,  -647,   690,   690,   690,   331,   334,   340,
     347,   351,  -647,   362,   363,   364,   365,   368,   369,   370,
     372,   373,   375,   377,   378,   385,   690,   690,  1365,   343,
    -647,   259,  -647,   261,    24,  -647,  -647,   462,  1665,   262,
    -647,  -647,   288,  -647,    31,  -647,  -647,  1665,   333,   103,
     103,   231,   114,   336,   289,   114,  -647,   690,  -647,   341,
      29,   -21,   287,  -647,  -647,  -647,   290,   292,   295,   299,
     302,  -647,  -647,     5,   123,    47,   303,  -647,  -647,   389,
       8,    38,   316,   427,   429,   690,    15,   -13,   690,   690,
    -647,   690,   690,  -647,  -647,  1002,   690,   690,   690,   690,
     690,   438,   439,   690,   440,   442,   443,   690,   690,   444,
     446,   690,   690,   448,  -647,  -647,   690,   690,   690,   690,
     690,   690,   690,   690,   690,   690,   690,   690,   690,   690,
     690,   690,   690,   690,   690,   690,   690,   690,  1665,   451,
     465,  -647,   467,   690,   690,  1665,    93,   470,  -647,   475,
    -647,   358,   366,  -647,  -647,   478,  -647,  -647,  -647,   -72,
    -647,  1665,   323,  -647,  -647,  -647,  -647,  -647,  -647,  -647,
    -647,   481,  -647,  -647,   882,   449,    19,  -647,  -647,  -647,
    -647,  -647,  -647,  -647,   153,  -647,   153,   357,  -647,  -647,
    -647,  -647,  -647,   450,    41,  -647,    21,  -647,  -647,  -647,
    1385,  -647,    -1,  1665,  1665,   605,  1665,  1665,  -647,   820,
    1022,  1405,  1425,  1042,   354,   361,  1062,   367,   379,   381,
    1445,  1465,   382,   383,  1082,  1504,   384,  1625,  1604,  1681,
    1696,   982,   777,   967,   967,   430,   430,   430,   430,   199,
     199,    43,    43,  -647,  -647,  -647,  1665,  1665,  1665,  -647,
    -647,  -647,  1665,  1665,  -647,  -647,  -647,  -647,   103,   120,
     114,   454,  -647,  -647,   -57,   538,   614,   538,   690,   380,
    -647,     4,   479,  -647,   349,  -647,  -647,  -647,  -647,    38,
    -647,  -647,  -647,   486,  -647,   387,   388,   522,  -647,  -647,
     690,  -647,  -647,   690,   690,  -647,   690,  -647,  -647,  -647,
    -647,  -647,   690,   690,  -647,  -647,  -647,   523,  -647,   690,
     392,   513,  -647,  -647,  -647,   218,   494,  1584,   516,   456,
    -647,  1645,   459,  -647,  1665,    23,   529,  -647,   540,     1,
    -647,   460,  -647,    25,    38,  -647,  -647,  -647,   411,  1102,
    1123,  1143,  1163,  1183,  1203,   414,  1665,   114,   507,   103,
     103,  -647,  -647,  -647,  -647,  -647,  -647,   434,   690,   187,
     563,  -647,   543,   544,  -647,  -647,   456,   528,   546,   547,
    -647,   452,  -647,  -647,  -647,   579,   455,  -647,    34,    38,
    -647,  -647,  -647,  -647,  -647,  -647,  -647,  -647,  -647,   458,
     392,  -647,  1223,  -647,   690,   554,   497,   497,  -647,   690,
      23,   690,   463,  -647,  -647,   509,  -647,    80,   114,   550,
     225,  1244,   690,   564,  -647,  -647,   400,  1264,  -647,  1284,
    -647,  -647,   594,  -647,  -647,  -647,   568,   591,  -647,  1304,
     690,   145,   555,  -647,  -647,    23,  -647,   690,  -647,  -647,
    1324,  -647,  -647,  -647,   557,  -647,  -647,  -647,  1344,  -647,
    -647,  -647,   582,   728,    32,   604,   775,  -647,  -647,  -647,
    -647,  -647,  -647,  -647,   584,   585,   587,  -647,  -647,   589,
     590,  -647,   255,  -647,   592,  -647,  -647,  -647,   728,   574,
     595,   -43,  -647,  -647,  -647,    59,    83,  -647,  -647,    56,
    -647,   596,  -647,   -45,   255,  -647,  -647,  -647,  -647,   578,
     609,   598,   599,   500,   601,   502,   610,   611,   505,   512,
    -647,    17,  -647,    16,   324,  -647,   255,   142,   609,   517,
     728,   646,   561,    56,    56,  -647,    56,  -647,    56,    56,
    -647,  -647,   519,   520,    56,  -647,  -647,  -647,   561,  -647,
     606,  -647,   633,  -647,   524,   530,    40,   531,   533,  -647,
    -647,  -647,  -647,   655,   106,   535,   536,    56,   539,   548,
     106,  -647,  -647,  -647,   661,  -647,  -647,  -647,   549,  -647,
    -647,  -647,   106,  -647,  -647,   455,  -647,   455,  -647,  -647,
    -647,   455,  -647
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -647,  -647,   -63,  -647,  -647,  -647,  -647,   445,  -647,  -647,
    -647,  -647,  -647,  -647,   562,  -647,  -647,   474,  -647,  -647,
    -647,  -203,  -647,  -647,  -647,  -647,   143,  -196,  -647,   888,
    -556,    14,    49,    33,  -647,  -647,    77,  -647,    42,  -647,
      -6,  -646,  -647,    79,  -523,  -212,  -647,  -647,  -294,  -647,
    -647,  -647,  -647,  -647,   234,  -647,  -647,  -647,  -647,  -647,
    -647,  -200,  -103,  -647,   -64,    45,   203,  -647,   176,  -647,
    -647,  -647,  -647,  -647,  -647,  -647,  -647,  -647,  -647,  -647,
    -647,  -647,  -647,  -647,  -446,   326,  -647,  -647,    73,  -602,
    -647,  -647,  -647,  -647,  -647,  -647,  -647,  -647,  -647,  -647,
    -493,  -647,  -647,  -647,  -647,   704,  -647,  -647,  -647,  -647,
    -647,   508,   -19,  -647,   638,    -9,  -647,  -647,   206
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -313
static const short yytable[] =
{
     196,   324,   326,   306,    97,   467,   140,   168,   467,   198,
      89,   500,   331,   332,   117,   328,   101,   178,   336,   288,
     630,   672,   415,   204,   187,   331,   332,   508,   281,   331,
     332,   597,   597,   308,   228,   288,   622,    17,   331,   332,
      21,   231,   217,   218,   630,   421,   422,   558,   598,   598,
     402,   215,    21,   428,   711,   597,   632,    20,   207,   208,
     630,   210,   212,   630,   403,   453,   718,   222,   631,   605,
     621,   597,   598,   413,   597,   272,   273,   274,   632,   454,
     233,   234,   586,   309,   331,   332,   623,   630,   598,   700,
     322,   598,   135,   136,   655,   621,   394,   395,   597,   712,
     676,    25,   254,   255,   567,   278,   333,    89,    88,    22,
     641,   642,   703,   285,   311,   598,   312,   509,   294,   333,
     472,    22,   704,   333,   294,    96,    90,   418,   113,    91,
      92,    93,   333,   301,   646,   647,   219,   621,   513,   423,
     322,   468,   323,   334,   468,   335,   595,   211,   114,   610,
     289,   643,   341,   674,   414,   612,   334,   117,   424,   282,
     334,   340,   517,   229,   343,   344,   289,   346,   347,   334,
     232,   546,   349,   350,   351,   352,   353,   697,   333,   356,
     291,   292,   327,   360,   361,   115,   116,   364,   365,   119,
     138,   644,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   120,   334,   473,   564,   139,   392,
     393,   142,   143,    90,   141,   121,    91,    98,    99,   270,
     271,   272,   273,   274,   295,   425,   426,   296,   297,   298,
     295,   122,   123,   296,   297,   451,   124,   125,   144,   145,
     491,   492,   493,   494,   495,   146,   147,   148,   322,   630,
     325,   416,   499,   417,   566,   126,   149,   150,   151,   127,
     597,   518,   128,   152,   581,   582,   583,   322,   153,   677,
     130,   491,   492,   493,   494,   495,   154,   598,   131,   132,
     134,   155,   156,   157,   158,   159,   160,     1,     2,     3,
     169,   170,   172,   161,   545,   162,   631,   605,     4,   175,
     176,   177,   179,   180,   191,     5,   547,   181,   183,   185,
     163,   186,   188,    97,   496,   189,   164,   190,   630,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   192,   597,
     194,   457,   461,   457,   464,   302,   142,   143,   195,   276,
     201,   165,   202,   204,   206,   496,   598,   209,   166,   167,
     306,   215,   221,   223,   224,   225,   479,   293,   236,   480,
     481,   237,   482,   144,   145,   641,   642,   238,   483,   484,
     146,   147,   148,   227,   239,   486,   290,   610,   240,   450,
     299,   149,   150,   151,   279,   303,   280,   286,   152,   241,
     242,   243,   244,   153,   302,   245,   246,   247,   304,   248,
     249,   154,   250,    40,   251,   252,   155,   156,   157,   158,
     159,   160,   253,   287,   313,   300,   330,   317,   161,   318,
     162,   305,   319,   338,   532,   339,   320,    50,    51,   321,
     329,   561,   354,   355,   357,   163,   358,   359,   362,    52,
     363,   164,   366,   337,   573,   389,   644,    67,   268,   269,
     270,   271,   272,   273,   274,   142,   143,   304,   283,   390,
     551,   391,    40,   587,   396,   557,   165,   559,   277,   397,
     529,   530,   401,   166,   167,   409,   411,   420,   569,   436,
     305,    68,   144,   145,   399,   471,    50,    51,   437,   146,
     147,   148,   400,   719,   439,   720,   580,   452,    52,   722,
     149,   150,   151,   588,    69,   466,   440,   152,   441,   444,
     445,   448,   153,   475,   476,   477,   478,   485,   487,   489,
     154,   498,   501,   511,   659,   155,   156,   157,   158,   159,
     160,   142,   143,   505,   512,   515,    70,   161,   520,   162,
     502,   527,    71,    72,    73,    74,    75,   -43,    76,    77,
      78,   528,    79,    80,   163,    81,    82,    83,   144,   145,
     164,   531,    84,    85,    86,   455,   147,   148,   456,   533,
     534,   538,   535,   539,   541,   544,   149,   150,   151,   543,
     322,   552,   553,   152,   548,   165,   562,   284,   153,   560,
     576,   570,   166,   167,   565,   577,   154,   578,   585,    67,
     590,   155,   156,   157,   158,   159,   160,   142,   143,   499,
     624,   625,   626,   161,   627,   162,   628,   629,   636,   634,
     460,   660,   637,   654,   661,   663,   664,   665,   666,   667,
     163,   429,   670,    68,   144,   145,   164,   668,   669,   671,
     681,   146,   147,   148,   679,   682,  -123,   689,   693,   702,
     692,   695,   149,   150,   151,   714,    69,   696,   698,   152,
     699,   165,   706,   707,   153,   345,   709,   314,   166,   167,
     686,   556,   154,   657,   673,   710,   716,   155,   156,   157,
     158,   159,   160,   142,   143,   635,   226,   713,    70,   161,
     638,   162,   680,   514,    71,    72,    73,    74,    75,   537,
      76,    77,    78,   555,    79,    80,   163,    81,    82,    83,
     144,   145,   164,   691,    84,    85,    86,   146,   147,   148,
     102,   678,   596,   463,   193,   342,   549,     0,   149,   150,
     151,     0,     0,   597,     0,   152,     0,   165,     0,     0,
     153,     0,     0,     0,   166,   167,     0,     0,   154,     0,
     598,     0,     0,   155,   156,   157,   158,   159,   160,   599,
     600,   601,   602,   603,     0,   161,     0,   162,     0,   604,
     605,   103,   104,   105,   106,   107,   108,   109,   110,   111,
       0,     0,   163,     0,     0,     0,     0,     0,   164,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,  -107,     0,   606,    28,   607,     0,     0,     0,
     608,     0,     0,   165,    50,    51,     0,     0,     0,     0,
     166,   167,     0,     0,     0,   256,     0,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,     0,   609,    29,    30,     0,
     610,     0,     0,     0,   611,     0,   612,    31,    32,    33,
      34,     0,    35,    36,     0,     0,     0,     0,     0,     0,
      37,    38,    39,    40,     0,     0,    28,     0,     0,     0,
      41,    42,    43,    44,    45,    46,     0,     0,     0,     0,
      47,    48,    49,     0,     0,     0,     0,    50,    51,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   410,    52,
       0,     0,     0,     0,     0,     0,     0,     0,    29,    30,
       0,    53,     0,     0,     0,     0,     0,  -312,    31,    32,
      33,    34,     0,    35,    36,     0,     0,    54,     0,     0,
       0,    37,    38,    39,    40,   430,     0,   431,     0,     0,
       0,    41,    42,    43,    44,    45,    46,     0,     0,     0,
       0,    47,    48,    49,     0,     0,     0,     0,    50,    51,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      52,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,    53,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   256,    54,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   256,     0,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   256,     0,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   256,     0,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   256,     0,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   256,     0,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,     0,   256,   348,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   256,   432,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   256,   435,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   256,   438,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   256,   446,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   256,   521,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,     0,   256,
     522,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   256,
     523,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   256,
     524,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   256,
     525,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   256,
     526,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   256,
     550,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,     0,
     256,   568,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     256,   574,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     256,   575,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     256,   579,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     256,   589,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     256,   592,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   645,   648,     0,     0,   652,     0,   256,
     427,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,     0,
     433,   675,   645,     0,     0,     0,     0,     0,     0,     0,
       0,   684,   685,     0,   652,     0,   687,   688,     0,     0,
     434,     0,   690,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   675,     0,     0,     0,     0,     0,
     442,     0,     0,     0,     0,   708,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   256,
     443,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,     0,
       0,   499,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   447,
     256,   449,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     256,   504,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     256,     0,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274
};

static const short yycheck[] =
{
     103,   213,   214,   199,    23,     4,    69,    71,     4,   112,
       4,   457,     4,     5,     4,   215,    25,    81,   221,     4,
       4,     4,   316,     4,    88,     4,     5,     4,     4,     4,
       5,    15,    15,     4,     6,     4,     4,     0,     4,     5,
      53,     6,     4,     5,     4,     4,     5,   540,    32,    32,
     122,     4,    53,    54,   700,    15,   612,     4,   121,   122,
       4,   124,   125,     4,   136,   122,   712,   130,    51,    52,
     593,    15,    32,    54,    15,    32,    33,    34,   634,   136,
     144,   145,   575,    54,     4,     5,    54,     4,    32,   691,
     135,    32,   135,   136,   139,   618,     3,     4,    15,   701,
     656,    53,   166,   167,   550,   169,    98,     4,     6,   122,
      51,    52,     6,   177,   135,    32,   137,    94,     4,    98,
     414,   122,    16,    98,     4,    53,   120,   327,    53,   123,
     124,   125,    98,   197,    51,    52,    98,   660,   137,    98,
     135,   140,   137,   135,   140,   137,   592,   137,    53,   132,
     135,    92,   137,   137,   135,   138,   135,     4,   137,   135,
     135,   225,   137,   135,   228,   229,   135,   231,   232,   135,
     135,   137,   236,   237,   238,   239,   240,   137,    98,   243,
     189,   190,   135,   247,   248,    37,    37,   251,   252,    53,
     126,   132,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,   276,   277,    37,   135,   419,   137,     4,   283,
     284,     3,     4,   120,     4,    37,   123,   124,   125,    30,
      31,    32,    33,    34,   120,   338,   339,   123,   124,   125,
     120,    37,    37,   123,   124,   125,    37,    37,    30,    31,
      63,    64,    65,    66,    67,    37,    38,    39,   135,     4,
     137,   324,    37,   326,    39,    37,    48,    49,    50,    37,
      15,   474,    37,    55,   129,   130,   131,   135,    60,   137,
      37,    63,    64,    65,    66,    67,    68,    32,    37,    37,
      37,    73,    74,    75,    76,    77,    78,   116,   117,   118,
       4,     4,     4,    85,   516,    87,    51,    52,   127,     4,
       4,     4,     4,     3,    54,   134,   519,     4,     4,     4,
     102,   104,     4,   342,   137,    16,   108,    16,     4,     6,
       7,     8,     9,    10,    11,    12,    13,    14,   136,    15,
      54,   405,   406,   407,   408,     4,     3,     4,   136,     6,
       4,   133,     4,     4,     4,   137,    32,     4,   140,   141,
     556,     4,    37,     4,     4,    37,   430,   136,    37,   433,
     434,    37,   436,    30,    31,    51,    52,    37,   442,   443,
      37,    38,    39,    53,    37,   449,    53,   132,    37,   398,
      54,    48,    49,    50,   135,    54,   135,   135,    55,    37,
      37,    37,    37,    60,     4,    37,    37,    37,    67,    37,
      37,    68,    37,    72,    37,    37,    73,    74,    75,    76,
      77,    78,    37,   135,   137,   136,    37,   137,    85,   137,
      87,    90,   137,     6,   498,     6,   137,    96,    97,   137,
     137,   544,     4,     4,     4,   102,     4,     4,     4,   108,
       4,   108,     4,   137,    54,     4,   132,     4,    28,    29,
      30,    31,    32,    33,    34,     3,     4,    67,     6,     4,
     534,     4,    72,   576,     4,   539,   133,   541,   135,     4,
     489,   490,     4,   140,   141,     4,    37,    37,   552,   135,
      90,    38,    30,    31,   136,    16,    96,    97,   137,    37,
      38,    39,   136,   715,   137,   717,   570,    53,   108,   721,
      48,    49,    50,   577,    61,   135,   137,    55,   137,   137,
     137,   137,    60,    37,   137,   137,     4,     4,   136,    16,
      68,    37,    16,     4,   637,    73,    74,    75,    76,    77,
      78,     3,     4,    84,     4,    85,    93,    85,   137,    87,
      94,   137,    99,   100,   101,   102,   103,   104,   105,   106,
     107,    54,   109,   110,   102,   112,   113,   114,    30,    31,
     108,   137,   119,   120,   121,    37,    38,    39,    40,    16,
      37,    53,    38,    37,    37,     6,    48,    49,    50,   137,
     135,    37,    95,    55,   136,   133,    87,   135,    60,   136,
       6,    37,   140,   141,    54,    37,    68,    16,    53,     4,
      53,    73,    74,    75,    76,    77,    78,     3,     4,    37,
      16,    37,    37,    85,    37,    87,    37,    37,    54,    37,
      16,    53,    37,    37,    25,    37,    37,   137,    37,   137,
     102,    36,   137,    38,    30,    31,   108,    37,    37,   137,
       4,    37,    38,    39,   137,    94,   137,   137,    25,     4,
      54,   137,    48,    49,    50,     4,    61,   137,   137,    55,
     137,   133,   137,   137,    60,   230,   137,   203,   140,   141,
     666,   538,    68,   634,   651,   137,   137,    73,    74,    75,
      76,    77,    78,     3,     4,   618,   134,   703,    93,    85,
     621,    87,   660,   469,    99,   100,   101,   102,   103,   506,
     105,   106,   107,   537,   109,   110,   102,   112,   113,   114,
      30,    31,   108,   678,   119,   120,   121,    37,    38,    39,
      26,   658,     4,   407,    96,   227,   530,    -1,    48,    49,
      50,    -1,    -1,    15,    -1,    55,    -1,   133,    -1,    -1,
      60,    -1,    -1,    -1,   140,   141,    -1,    -1,    68,    -1,
      32,    -1,    -1,    73,    74,    75,    76,    77,    78,    41,
      42,    43,    44,    45,    -1,    85,    -1,    87,    -1,    51,
      52,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,   102,    -1,    -1,    -1,    -1,    -1,   108,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    37,    -1,    86,     4,    88,    -1,    -1,    -1,
      92,    -1,    -1,   133,    96,    97,    -1,    -1,    -1,    -1,
     140,   141,    -1,    -1,    -1,    15,    -1,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,   128,    46,    47,    -1,
     132,    -1,    -1,    -1,   136,    -1,   138,    56,    57,    58,
      59,    -1,    61,    62,    -1,    -1,    -1,    -1,    -1,    -1,
      69,    70,    71,    72,    -1,    -1,     4,    -1,    -1,    -1,
      79,    80,    81,    82,    83,    84,    -1,    -1,    -1,    -1,
      89,    90,    91,    -1,    -1,    -1,    -1,    96,    97,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,   108,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    47,
      -1,   120,    -1,    -1,    -1,    -1,    -1,   126,    56,    57,
      58,    59,    -1,    61,    62,    -1,    -1,   136,    -1,    -1,
      -1,    69,    70,    71,    72,   135,    -1,   137,    -1,    -1,
      -1,    79,    80,    81,    82,    83,    84,    -1,    -1,    -1,
      -1,    89,    90,    91,    -1,    -1,    -1,    -1,    96,    97,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     108,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,   120,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    15,   136,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    15,    -1,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    15,    -1,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    15,    -1,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    15,    -1,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    15,    -1,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    -1,    15,   137,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    15,   137,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    15,   137,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    15,   137,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    15,   137,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    15,   137,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    15,
     137,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    15,
     137,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    15,
     137,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    15,
     137,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    15,
     137,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    15,
     137,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    -1,
      15,   137,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   137,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   137,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   137,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   137,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   137,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
     135,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   625,   626,    -1,    -1,   629,    -1,    15,
     135,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    -1,
     135,   653,   654,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   663,   664,    -1,   666,    -1,   668,   669,    -1,    -1,
     135,    -1,   674,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   686,    -1,    -1,    -1,    -1,    -1,
     135,    -1,    -1,    -1,    -1,   697,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,
     135,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    -1,
      -1,    37,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,   135,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,   116,   117,   118,   127,   134,   143,   157,   158,   147,
     148,   145,   146,   249,   250,   244,   245,     0,   159,   149,
       4,    53,   122,   253,   254,    53,   246,   247,     4,    46,
      47,    56,    57,    58,    59,    61,    62,    69,    70,    71,
      72,    79,    80,    81,    82,    83,    84,    89,    90,    91,
      96,    97,   108,   120,   136,   160,   167,   169,   186,   188,
     197,   198,   200,   202,   236,   251,   252,     4,    38,    61,
      93,    99,   100,   101,   102,   103,   105,   106,   107,   109,
     110,   112,   113,   114,   119,   120,   121,   150,     6,     4,
     120,   123,   124,   125,   256,   257,    53,   254,   124,   125,
     248,   257,   247,     6,     7,     8,     9,    10,    11,    12,
      13,    14,   184,    53,    53,    37,    37,     4,   144,    53,
      37,    37,    37,    37,    37,    37,    37,    37,    37,   161,
      37,    37,    37,   170,    37,   135,   136,   185,   126,     4,
     144,     4,     3,     4,    30,    31,    37,    38,    39,    48,
      49,    50,    55,    60,    68,    73,    74,    75,    76,    77,
      78,    85,    87,   102,   108,   133,   140,   141,   206,     4,
       4,   154,     4,   153,   152,     4,     4,     4,   206,     4,
       3,     4,   155,     4,   156,     4,   104,   206,     4,    16,
      16,    54,   136,   256,    54,   136,   204,   205,   204,   168,
     237,     4,     4,   162,     4,   190,     4,   144,   144,     4,
     144,   137,   144,   199,   201,     4,   203,     4,     5,    98,
     163,    37,   144,     4,     4,    37,   156,    53,     6,   135,
     151,     6,   135,   206,   206,   206,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,   206,   206,    15,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,   135,     6,   135,   206,   135,
     135,     4,   135,     6,   135,   206,   135,   135,     4,   135,
      53,   257,   257,   136,     4,   120,   123,   124,   125,    54,
     136,   206,     4,    54,    67,    90,   169,   212,     4,    54,
     238,   135,   137,   137,   159,   191,   189,   137,   137,   137,
     137,   137,   135,   137,   187,   137,   187,   135,   203,   137,
      37,     4,     5,    98,   135,   137,   163,   137,     6,     6,
     206,   137,   253,   206,   206,   149,   206,   206,   137,   206,
     206,   206,   206,   206,     4,     4,   206,     4,     4,     4,
     206,   206,     4,     4,   206,   206,     4,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,     4,
       4,     4,   206,   206,     3,     4,     4,     4,   259,   136,
     136,     4,   122,   136,   255,   213,   218,   223,   239,     4,
      36,    37,   194,    54,   135,   190,   144,   144,   203,   164,
      37,     4,     5,    98,   137,   204,   204,   135,    54,    36,
     135,   137,   137,   135,   135,   137,   135,   137,   137,   137,
     137,   137,   135,   135,   137,   137,   137,   135,   137,    16,
     257,   125,    53,   122,   136,    37,    40,   206,   226,   227,
      16,   206,   228,   227,   206,   241,   135,     4,   140,   195,
     196,    16,   190,   163,   166,    37,   137,   137,     4,   206,
     206,   206,   206,   206,   206,     4,   206,   136,   260,    16,
     258,    63,    64,    65,    66,    67,   137,   225,    37,    37,
     226,    16,    94,   208,    16,    84,   229,   224,     4,    94,
     242,     4,     4,   137,   196,    85,   192,   137,   163,   165,
     137,   137,   137,   137,   137,   137,   137,   137,    54,   257,
     257,   137,   206,    16,    37,    38,   209,   208,    53,    37,
     243,    37,   240,   137,     6,   187,   137,   163,   136,   260,
     137,   206,    37,    95,   210,   210,   168,   206,   242,   206,
     136,   204,    87,   193,   137,    54,    39,   226,   137,   206,
      37,   214,   219,    54,   137,   137,     6,    37,    16,   137,
     206,   129,   130,   131,   211,    53,   242,   204,   206,   137,
      53,   220,   137,   215,   232,   226,     4,    15,    32,    41,
      42,    43,    44,    45,    51,    52,    86,    88,    92,   128,
     132,   136,   138,   171,   172,   175,   176,   178,   179,   180,
     181,   186,     4,    54,    16,    37,    37,    37,    37,    37,
       4,    51,   172,   174,    37,   178,    54,    37,   185,   233,
     221,    51,    52,    92,   132,   171,    51,    52,   171,   182,
     204,   177,   171,   173,    37,   139,   187,   174,   216,   204,
      53,    25,   230,    37,    37,   137,    37,   137,    37,    37,
     137,   137,     4,   175,   137,   171,   172,   137,   230,   137,
     180,     4,    94,   207,   171,   171,   173,   171,   171,   137,
     171,   207,    54,    25,   231,   137,   137,   137,   137,   137,
     231,   234,     4,     6,    16,   183,   137,   137,   171,   137,
     137,   183,   231,   182,     4,   222,   137,   217,   183,   187,
     187,   235,   187
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

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
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
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
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
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

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
        case 8:
#line 175 "ldgram.y"
    { ldlex_defsym(); }
    break;

  case 9:
#line 177 "ldgram.y"
    {
		  ldlex_popstate();
		  lang_add_assignment(exp_assop(yyvsp[-1].token,yyvsp[-2].name,yyvsp[0].etree));
		}
    break;

  case 10:
#line 185 "ldgram.y"
    {
		  ldlex_mri_script ();
		  PUSH_ERROR (_("MRI style script"));
		}
    break;

  case 11:
#line 190 "ldgram.y"
    {
		  ldlex_popstate ();
		  mri_draw_tree ();
		  POP_ERROR ();
		}
    break;

  case 16:
#line 205 "ldgram.y"
    {
			einfo(_("%P%F: unrecognised keyword in MRI style script '%s'\n"),yyvsp[0].name);
			}
    break;

  case 17:
#line 208 "ldgram.y"
    {
			config.map_filename = "-";
			}
    break;

  case 20:
#line 214 "ldgram.y"
    { mri_public(yyvsp[-2].name, yyvsp[0].etree); }
    break;

  case 21:
#line 216 "ldgram.y"
    { mri_public(yyvsp[-2].name, yyvsp[0].etree); }
    break;

  case 22:
#line 218 "ldgram.y"
    { mri_public(yyvsp[-1].name, yyvsp[0].etree); }
    break;

  case 23:
#line 220 "ldgram.y"
    { mri_format(yyvsp[0].name); }
    break;

  case 24:
#line 222 "ldgram.y"
    { mri_output_section(yyvsp[-2].name, yyvsp[0].etree);}
    break;

  case 25:
#line 224 "ldgram.y"
    { mri_output_section(yyvsp[-1].name, yyvsp[0].etree);}
    break;

  case 26:
#line 226 "ldgram.y"
    { mri_output_section(yyvsp[-2].name, yyvsp[0].etree);}
    break;

  case 27:
#line 228 "ldgram.y"
    { mri_align(yyvsp[-2].name,yyvsp[0].etree); }
    break;

  case 28:
#line 230 "ldgram.y"
    { mri_align(yyvsp[-2].name,yyvsp[0].etree); }
    break;

  case 29:
#line 232 "ldgram.y"
    { mri_alignmod(yyvsp[-2].name,yyvsp[0].etree); }
    break;

  case 30:
#line 234 "ldgram.y"
    { mri_alignmod(yyvsp[-2].name,yyvsp[0].etree); }
    break;

  case 33:
#line 238 "ldgram.y"
    { mri_name(yyvsp[0].name); }
    break;

  case 34:
#line 240 "ldgram.y"
    { mri_alias(yyvsp[-2].name,yyvsp[0].name,0);}
    break;

  case 35:
#line 242 "ldgram.y"
    { mri_alias (yyvsp[-2].name, 0, (int) yyvsp[0].bigint.integer); }
    break;

  case 36:
#line 244 "ldgram.y"
    { mri_base(yyvsp[0].etree); }
    break;

  case 37:
#line 246 "ldgram.y"
    { mri_truncate ((unsigned int) yyvsp[0].bigint.integer); }
    break;

  case 40:
#line 250 "ldgram.y"
    { ldlex_script (); ldfile_open_command_file(yyvsp[0].name); }
    break;

  case 41:
#line 252 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 42:
#line 254 "ldgram.y"
    { lang_add_entry (yyvsp[0].name, FALSE); }
    break;

  case 44:
#line 259 "ldgram.y"
    { mri_order(yyvsp[0].name); }
    break;

  case 45:
#line 260 "ldgram.y"
    { mri_order(yyvsp[0].name); }
    break;

  case 47:
#line 266 "ldgram.y"
    { mri_load(yyvsp[0].name); }
    break;

  case 48:
#line 267 "ldgram.y"
    { mri_load(yyvsp[0].name); }
    break;

  case 49:
#line 272 "ldgram.y"
    { mri_only_load(yyvsp[0].name); }
    break;

  case 50:
#line 274 "ldgram.y"
    { mri_only_load(yyvsp[0].name); }
    break;

  case 51:
#line 278 "ldgram.y"
    { yyval.name = NULL; }
    break;

  case 54:
#line 285 "ldgram.y"
    { ldlang_add_undef (yyvsp[0].name); }
    break;

  case 55:
#line 287 "ldgram.y"
    { ldlang_add_undef (yyvsp[0].name); }
    break;

  case 56:
#line 289 "ldgram.y"
    { ldlang_add_undef (yyvsp[0].name); }
    break;

  case 57:
#line 293 "ldgram.y"
    {
	 ldlex_both();
	}
    break;

  case 58:
#line 297 "ldgram.y"
    {
	ldlex_popstate();
	}
    break;

  case 71:
#line 322 "ldgram.y"
    { lang_add_target(yyvsp[-1].name); }
    break;

  case 72:
#line 324 "ldgram.y"
    { ldfile_add_library_path (yyvsp[-1].name, FALSE); }
    break;

  case 73:
#line 326 "ldgram.y"
    { lang_add_output(yyvsp[-1].name, 1); }
    break;

  case 74:
#line 328 "ldgram.y"
    { lang_add_output_format (yyvsp[-1].name, (char *) NULL,
					    (char *) NULL, 1); }
    break;

  case 75:
#line 331 "ldgram.y"
    { lang_add_output_format (yyvsp[-5].name, yyvsp[-3].name, yyvsp[-1].name, 1); }
    break;

  case 76:
#line 333 "ldgram.y"
    { ldfile_set_output_arch (yyvsp[-1].name, bfd_arch_unknown); }
    break;

  case 77:
#line 335 "ldgram.y"
    { command_line.force_common_definition = TRUE ; }
    break;

  case 78:
#line 337 "ldgram.y"
    { command_line.inhibit_common_definition = TRUE ; }
    break;

  case 80:
#line 340 "ldgram.y"
    { lang_enter_group (); }
    break;

  case 81:
#line 342 "ldgram.y"
    { lang_leave_group (); }
    break;

  case 82:
#line 344 "ldgram.y"
    { lang_add_map(yyvsp[-1].name); }
    break;

  case 83:
#line 346 "ldgram.y"
    { ldlex_script (); ldfile_open_command_file(yyvsp[0].name); }
    break;

  case 84:
#line 348 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 85:
#line 350 "ldgram.y"
    {
		  lang_add_nocrossref (yyvsp[-1].nocrossref);
		}
    break;

  case 87:
#line 358 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 88:
#line 361 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 89:
#line 364 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 90:
#line 367 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 91:
#line 370 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 92:
#line 373 "ldgram.y"
    { lang_add_input_file(yyvsp[0].name,lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 93:
#line 376 "ldgram.y"
    { yyval.integer = as_needed; as_needed = TRUE; }
    break;

  case 94:
#line 378 "ldgram.y"
    { as_needed = yyvsp[-2].integer; }
    break;

  case 95:
#line 380 "ldgram.y"
    { yyval.integer = as_needed; as_needed = TRUE; }
    break;

  case 96:
#line 382 "ldgram.y"
    { as_needed = yyvsp[-2].integer; }
    break;

  case 97:
#line 384 "ldgram.y"
    { yyval.integer = as_needed; as_needed = TRUE; }
    break;

  case 98:
#line 386 "ldgram.y"
    { as_needed = yyvsp[-2].integer; }
    break;

  case 103:
#line 401 "ldgram.y"
    { lang_add_entry (yyvsp[-1].name, FALSE); }
    break;

  case 105:
#line 403 "ldgram.y"
    {ldlex_expression ();}
    break;

  case 106:
#line 404 "ldgram.y"
    { ldlex_popstate ();
		  lang_add_assignment (exp_assert (yyvsp[-3].etree, yyvsp[-1].name)); }
    break;

  case 107:
#line 412 "ldgram.y"
    {
			  yyval.cname = yyvsp[0].name;
			}
    break;

  case 108:
#line 416 "ldgram.y"
    {
			  yyval.cname = "*";
			}
    break;

  case 109:
#line 420 "ldgram.y"
    {
			  yyval.cname = "?";
			}
    break;

  case 110:
#line 427 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[0].cname;
			  yyval.wildcard.sorted = none;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 111:
#line 433 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[0].cname;
			  yyval.wildcard.sorted = none;
			  yyval.wildcard.exclude_name_list = yyvsp[-2].name_list;
			}
    break;

  case 112:
#line 439 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-1].cname;
			  yyval.wildcard.sorted = by_name;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 113:
#line 445 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-1].cname;
			  yyval.wildcard.sorted = by_alignment;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 114:
#line 451 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-2].cname;
			  yyval.wildcard.sorted = by_name_alignment;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 115:
#line 457 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-2].cname;
			  yyval.wildcard.sorted = by_name;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 116:
#line 463 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-2].cname;
			  yyval.wildcard.sorted = by_alignment_name;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 117:
#line 469 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-2].cname;
			  yyval.wildcard.sorted = by_alignment;
			  yyval.wildcard.exclude_name_list = NULL;
			}
    break;

  case 118:
#line 475 "ldgram.y"
    {
			  yyval.wildcard.name = yyvsp[-1].cname;
			  yyval.wildcard.sorted = by_name;
			  yyval.wildcard.exclude_name_list = yyvsp[-3].name_list;
			}
    break;

  case 119:
#line 484 "ldgram.y"
    {
			  struct name_list *tmp;
			  tmp = (struct name_list *) xmalloc (sizeof *tmp);
			  tmp->name = yyvsp[0].cname;
			  tmp->next = yyvsp[-1].name_list;
			  yyval.name_list = tmp;
			}
    break;

  case 120:
#line 493 "ldgram.y"
    {
			  struct name_list *tmp;
			  tmp = (struct name_list *) xmalloc (sizeof *tmp);
			  tmp->name = yyvsp[0].cname;
			  tmp->next = NULL;
			  yyval.name_list = tmp;
			}
    break;

  case 121:
#line 504 "ldgram.y"
    {
			  struct wildcard_list *tmp;
			  tmp = (struct wildcard_list *) xmalloc (sizeof *tmp);
			  tmp->next = yyvsp[-2].wildcard_list;
			  tmp->spec = yyvsp[0].wildcard;
			  yyval.wildcard_list = tmp;
			}
    break;

  case 122:
#line 513 "ldgram.y"
    {
			  struct wildcard_list *tmp;
			  tmp = (struct wildcard_list *) xmalloc (sizeof *tmp);
			  tmp->next = NULL;
			  tmp->spec = yyvsp[0].wildcard;
			  yyval.wildcard_list = tmp;
			}
    break;

  case 123:
#line 524 "ldgram.y"
    {
			  struct wildcard_spec tmp;
			  tmp.name = yyvsp[0].name;
			  tmp.exclude_name_list = NULL;
			  tmp.sorted = none;
			  lang_add_wild (&tmp, NULL, ldgram_had_keep);
			}
    break;

  case 124:
#line 532 "ldgram.y"
    {
			  lang_add_wild (NULL, yyvsp[-1].wildcard_list, ldgram_had_keep);
			}
    break;

  case 125:
#line 536 "ldgram.y"
    {
			  lang_add_wild (&yyvsp[-3].wildcard, yyvsp[-1].wildcard_list, ldgram_had_keep);
			}
    break;

  case 127:
#line 544 "ldgram.y"
    { ldgram_had_keep = TRUE; }
    break;

  case 128:
#line 546 "ldgram.y"
    { ldgram_had_keep = FALSE; }
    break;

  case 130:
#line 552 "ldgram.y"
    {
 		lang_add_attribute(lang_object_symbols_statement_enum);
	      	}
    break;

  case 132:
#line 557 "ldgram.y"
    {

		  lang_add_attribute(lang_constructors_statement_enum);
		}
    break;

  case 133:
#line 562 "ldgram.y"
    {
		  constructors_sorted = TRUE;
		  lang_add_attribute (lang_constructors_statement_enum);
		}
    break;

  case 135:
#line 568 "ldgram.y"
    {
			  lang_add_data ((int) yyvsp[-3].integer, yyvsp[-1].etree);
			}
    break;

  case 136:
#line 573 "ldgram.y"
    {
			  lang_add_fill (yyvsp[-1].fill);
			}
    break;

  case 141:
#line 590 "ldgram.y"
    { yyval.integer = yyvsp[0].token; }
    break;

  case 142:
#line 592 "ldgram.y"
    { yyval.integer = yyvsp[0].token; }
    break;

  case 143:
#line 594 "ldgram.y"
    { yyval.integer = yyvsp[0].token; }
    break;

  case 144:
#line 596 "ldgram.y"
    { yyval.integer = yyvsp[0].token; }
    break;

  case 145:
#line 598 "ldgram.y"
    { yyval.integer = yyvsp[0].token; }
    break;

  case 146:
#line 603 "ldgram.y"
    {
		  yyval.fill = exp_get_fill (yyvsp[0].etree, 0, "fill value");
		}
    break;

  case 147:
#line 610 "ldgram.y"
    { yyval.fill = yyvsp[0].fill; }
    break;

  case 148:
#line 611 "ldgram.y"
    { yyval.fill = (fill_type *) 0; }
    break;

  case 149:
#line 616 "ldgram.y"
    { yyval.token = '+'; }
    break;

  case 150:
#line 618 "ldgram.y"
    { yyval.token = '-'; }
    break;

  case 151:
#line 620 "ldgram.y"
    { yyval.token = '*'; }
    break;

  case 152:
#line 622 "ldgram.y"
    { yyval.token = '/'; }
    break;

  case 153:
#line 624 "ldgram.y"
    { yyval.token = LSHIFT; }
    break;

  case 154:
#line 626 "ldgram.y"
    { yyval.token = RSHIFT; }
    break;

  case 155:
#line 628 "ldgram.y"
    { yyval.token = '&'; }
    break;

  case 156:
#line 630 "ldgram.y"
    { yyval.token = '|'; }
    break;

  case 159:
#line 640 "ldgram.y"
    {
		  lang_add_assignment (exp_assop (yyvsp[-1].token, yyvsp[-2].name, yyvsp[0].etree));
		}
    break;

  case 160:
#line 644 "ldgram.y"
    {
		  lang_add_assignment (exp_assop ('=', yyvsp[-2].name,
						  exp_binop (yyvsp[-1].token,
							     exp_nameop (NAME,
									 yyvsp[-2].name),
							     yyvsp[0].etree)));
		}
    break;

  case 161:
#line 652 "ldgram.y"
    {
		  lang_add_assignment (exp_provide (yyvsp[-3].name, yyvsp[-1].etree, FALSE));
		}
    break;

  case 162:
#line 656 "ldgram.y"
    {
		  lang_add_assignment (exp_provide (yyvsp[-3].name, yyvsp[-1].etree, TRUE));
		}
    break;

  case 169:
#line 678 "ldgram.y"
    { region = lang_memory_region_lookup (yyvsp[0].name, TRUE); }
    break;

  case 170:
#line 681 "ldgram.y"
    {}
    break;

  case 171:
#line 686 "ldgram.y"
    {
		  region->origin = exp_get_vma (yyvsp[0].etree, 0, "origin");
		  region->current = region->origin;
		}
    break;

  case 172:
#line 694 "ldgram.y"
    {
		  region->length = exp_get_vma (yyvsp[0].etree, -1, "length");
		}
    break;

  case 173:
#line 701 "ldgram.y"
    { /* dummy action to avoid bison 1.25 error message */ }
    break;

  case 177:
#line 712 "ldgram.y"
    { lang_set_flags (region, yyvsp[0].name, 0); }
    break;

  case 178:
#line 714 "ldgram.y"
    { lang_set_flags (region, yyvsp[0].name, 1); }
    break;

  case 179:
#line 719 "ldgram.y"
    { lang_startup(yyvsp[-1].name); }
    break;

  case 181:
#line 725 "ldgram.y"
    { ldemul_hll((char *)NULL); }
    break;

  case 182:
#line 730 "ldgram.y"
    { ldemul_hll(yyvsp[0].name); }
    break;

  case 183:
#line 732 "ldgram.y"
    { ldemul_hll(yyvsp[0].name); }
    break;

  case 185:
#line 740 "ldgram.y"
    { ldemul_syslib(yyvsp[0].name); }
    break;

  case 187:
#line 746 "ldgram.y"
    { lang_float(TRUE); }
    break;

  case 188:
#line 748 "ldgram.y"
    { lang_float(FALSE); }
    break;

  case 189:
#line 753 "ldgram.y"
    {
		  yyval.nocrossref = NULL;
		}
    break;

  case 190:
#line 757 "ldgram.y"
    {
		  struct lang_nocrossref *n;

		  n = (struct lang_nocrossref *) xmalloc (sizeof *n);
		  n->name = yyvsp[-1].name;
		  n->next = yyvsp[0].nocrossref;
		  yyval.nocrossref = n;
		}
    break;

  case 191:
#line 766 "ldgram.y"
    {
		  struct lang_nocrossref *n;

		  n = (struct lang_nocrossref *) xmalloc (sizeof *n);
		  n->name = yyvsp[-2].name;
		  n->next = yyvsp[0].nocrossref;
		  yyval.nocrossref = n;
		}
    break;

  case 192:
#line 776 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 193:
#line 778 "ldgram.y"
    { ldlex_popstate (); yyval.etree=yyvsp[0].etree;}
    break;

  case 194:
#line 783 "ldgram.y"
    { yyval.etree = exp_unop ('-', yyvsp[0].etree); }
    break;

  case 195:
#line 785 "ldgram.y"
    { yyval.etree = yyvsp[-1].etree; }
    break;

  case 196:
#line 787 "ldgram.y"
    { yyval.etree = exp_unop ((int) yyvsp[-3].integer,yyvsp[-1].etree); }
    break;

  case 197:
#line 789 "ldgram.y"
    { yyval.etree = exp_unop ('!', yyvsp[0].etree); }
    break;

  case 198:
#line 791 "ldgram.y"
    { yyval.etree = yyvsp[0].etree; }
    break;

  case 199:
#line 793 "ldgram.y"
    { yyval.etree = exp_unop ('~', yyvsp[0].etree);}
    break;

  case 200:
#line 796 "ldgram.y"
    { yyval.etree = exp_binop ('*', yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 201:
#line 798 "ldgram.y"
    { yyval.etree = exp_binop ('/', yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 202:
#line 800 "ldgram.y"
    { yyval.etree = exp_binop ('%', yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 203:
#line 802 "ldgram.y"
    { yyval.etree = exp_binop ('+', yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 204:
#line 804 "ldgram.y"
    { yyval.etree = exp_binop ('-' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 205:
#line 806 "ldgram.y"
    { yyval.etree = exp_binop (LSHIFT , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 206:
#line 808 "ldgram.y"
    { yyval.etree = exp_binop (RSHIFT , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 207:
#line 810 "ldgram.y"
    { yyval.etree = exp_binop (EQ , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 208:
#line 812 "ldgram.y"
    { yyval.etree = exp_binop (NE , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 209:
#line 814 "ldgram.y"
    { yyval.etree = exp_binop (LE , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 210:
#line 816 "ldgram.y"
    { yyval.etree = exp_binop (GE , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 211:
#line 818 "ldgram.y"
    { yyval.etree = exp_binop ('<' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 212:
#line 820 "ldgram.y"
    { yyval.etree = exp_binop ('>' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 213:
#line 822 "ldgram.y"
    { yyval.etree = exp_binop ('&' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 214:
#line 824 "ldgram.y"
    { yyval.etree = exp_binop ('^' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 215:
#line 826 "ldgram.y"
    { yyval.etree = exp_binop ('|' , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 216:
#line 828 "ldgram.y"
    { yyval.etree = exp_trinop ('?' , yyvsp[-4].etree, yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 217:
#line 830 "ldgram.y"
    { yyval.etree = exp_binop (ANDAND , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 218:
#line 832 "ldgram.y"
    { yyval.etree = exp_binop (OROR , yyvsp[-2].etree, yyvsp[0].etree); }
    break;

  case 219:
#line 834 "ldgram.y"
    { yyval.etree = exp_nameop (DEFINED, yyvsp[-1].name); }
    break;

  case 220:
#line 836 "ldgram.y"
    { yyval.etree = exp_bigintop (yyvsp[0].bigint.integer, yyvsp[0].bigint.str); }
    break;

  case 221:
#line 838 "ldgram.y"
    { yyval.etree = exp_nameop (SIZEOF_HEADERS,0); }
    break;

  case 222:
#line 841 "ldgram.y"
    { yyval.etree = exp_nameop (SIZEOF,yyvsp[-1].name); }
    break;

  case 223:
#line 843 "ldgram.y"
    { yyval.etree = exp_nameop (ADDR,yyvsp[-1].name); }
    break;

  case 224:
#line 845 "ldgram.y"
    { yyval.etree = exp_nameop (LOADADDR,yyvsp[-1].name); }
    break;

  case 225:
#line 847 "ldgram.y"
    { yyval.etree = exp_nameop (CONSTANT,yyvsp[-1].name); }
    break;

  case 226:
#line 849 "ldgram.y"
    { yyval.etree = exp_unop (ABSOLUTE, yyvsp[-1].etree); }
    break;

  case 227:
#line 851 "ldgram.y"
    { yyval.etree = exp_unop (ALIGN_K,yyvsp[-1].etree); }
    break;

  case 228:
#line 853 "ldgram.y"
    { yyval.etree = exp_binop (ALIGN_K,yyvsp[-3].etree,yyvsp[-1].etree); }
    break;

  case 229:
#line 855 "ldgram.y"
    { yyval.etree = exp_binop (DATA_SEGMENT_ALIGN, yyvsp[-3].etree, yyvsp[-1].etree); }
    break;

  case 230:
#line 857 "ldgram.y"
    { yyval.etree = exp_binop (DATA_SEGMENT_RELRO_END, yyvsp[-1].etree, yyvsp[-3].etree); }
    break;

  case 231:
#line 859 "ldgram.y"
    { yyval.etree = exp_unop (DATA_SEGMENT_END, yyvsp[-1].etree); }
    break;

  case 232:
#line 861 "ldgram.y"
    { /* The operands to the expression node are
			     placed in the opposite order from the way
			     in which they appear in the script as
			     that allows us to reuse more code in
			     fold_binary.  */
			  yyval.etree = exp_binop (SEGMENT_START,
					  yyvsp[-1].etree,
					  exp_nameop (NAME, yyvsp[-3].name)); }
    break;

  case 233:
#line 870 "ldgram.y"
    { yyval.etree = exp_unop (ALIGN_K,yyvsp[-1].etree); }
    break;

  case 234:
#line 872 "ldgram.y"
    { yyval.etree = exp_nameop (NAME,yyvsp[0].name); }
    break;

  case 235:
#line 874 "ldgram.y"
    { yyval.etree = exp_binop (MAX_K, yyvsp[-3].etree, yyvsp[-1].etree ); }
    break;

  case 236:
#line 876 "ldgram.y"
    { yyval.etree = exp_binop (MIN_K, yyvsp[-3].etree, yyvsp[-1].etree ); }
    break;

  case 237:
#line 878 "ldgram.y"
    { yyval.etree = exp_assert (yyvsp[-3].etree, yyvsp[-1].name); }
    break;

  case 238:
#line 880 "ldgram.y"
    { yyval.etree = exp_nameop (ORIGIN, yyvsp[-1].name); }
    break;

  case 239:
#line 882 "ldgram.y"
    { yyval.etree = exp_nameop (LENGTH, yyvsp[-1].name); }
    break;

  case 240:
#line 887 "ldgram.y"
    { yyval.name = yyvsp[0].name; }
    break;

  case 241:
#line 888 "ldgram.y"
    { yyval.name = 0; }
    break;

  case 242:
#line 892 "ldgram.y"
    { yyval.etree = yyvsp[-1].etree; }
    break;

  case 243:
#line 893 "ldgram.y"
    { yyval.etree = 0; }
    break;

  case 244:
#line 897 "ldgram.y"
    { yyval.etree = yyvsp[-1].etree; }
    break;

  case 245:
#line 898 "ldgram.y"
    { yyval.etree = 0; }
    break;

  case 246:
#line 902 "ldgram.y"
    { yyval.etree = yyvsp[-1].etree; }
    break;

  case 247:
#line 903 "ldgram.y"
    { yyval.etree = 0; }
    break;

  case 248:
#line 907 "ldgram.y"
    { yyval.token = ONLY_IF_RO; }
    break;

  case 249:
#line 908 "ldgram.y"
    { yyval.token = ONLY_IF_RW; }
    break;

  case 250:
#line 909 "ldgram.y"
    { yyval.token = SPECIAL; }
    break;

  case 251:
#line 910 "ldgram.y"
    { yyval.token = 0; }
    break;

  case 252:
#line 913 "ldgram.y"
    { ldlex_expression(); }
    break;

  case 253:
#line 917 "ldgram.y"
    { ldlex_popstate (); ldlex_script (); }
    break;

  case 254:
#line 920 "ldgram.y"
    {
			  lang_enter_output_section_statement(yyvsp[-8].name, yyvsp[-6].etree,
							      sectype,
							      yyvsp[-4].etree, yyvsp[-3].etree, yyvsp[-5].etree, yyvsp[-1].token);
			}
    break;

  case 255:
#line 926 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 256:
#line 928 "ldgram.y"
    {
		  ldlex_popstate ();
		  lang_leave_output_section_statement (yyvsp[0].fill, yyvsp[-3].name, yyvsp[-1].section_phdr, yyvsp[-2].name);
		}
    break;

  case 257:
#line 933 "ldgram.y"
    {}
    break;

  case 258:
#line 935 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 259:
#line 937 "ldgram.y"
    { ldlex_popstate (); ldlex_script (); }
    break;

  case 260:
#line 939 "ldgram.y"
    {
			  lang_enter_overlay (yyvsp[-5].etree, yyvsp[-2].etree);
			}
    break;

  case 261:
#line 944 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 262:
#line 946 "ldgram.y"
    {
			  ldlex_popstate ();
			  lang_leave_overlay (yyvsp[-11].etree, (int) yyvsp[-12].integer,
					      yyvsp[0].fill, yyvsp[-3].name, yyvsp[-1].section_phdr, yyvsp[-2].name);
			}
    break;

  case 264:
#line 956 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 265:
#line 958 "ldgram.y"
    {
		  ldlex_popstate ();
		  lang_add_assignment (exp_assop ('=', ".", yyvsp[0].etree));
		}
    break;

  case 267:
#line 966 "ldgram.y"
    { sectype = noload_section; }
    break;

  case 268:
#line 967 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 269:
#line 968 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 270:
#line 969 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 271:
#line 970 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 273:
#line 975 "ldgram.y"
    { sectype = normal_section; }
    break;

  case 274:
#line 976 "ldgram.y"
    { sectype = normal_section; }
    break;

  case 275:
#line 980 "ldgram.y"
    { yyval.etree = yyvsp[-2].etree; }
    break;

  case 276:
#line 981 "ldgram.y"
    { yyval.etree = (etree_type *)NULL;  }
    break;

  case 277:
#line 986 "ldgram.y"
    { yyval.etree = yyvsp[-3].etree; }
    break;

  case 278:
#line 988 "ldgram.y"
    { yyval.etree = yyvsp[-7].etree; }
    break;

  case 279:
#line 992 "ldgram.y"
    { yyval.etree = yyvsp[-1].etree; }
    break;

  case 280:
#line 993 "ldgram.y"
    { yyval.etree = (etree_type *) NULL;  }
    break;

  case 281:
#line 998 "ldgram.y"
    { yyval.integer = 0; }
    break;

  case 282:
#line 1000 "ldgram.y"
    { yyval.integer = 1; }
    break;

  case 283:
#line 1005 "ldgram.y"
    { yyval.name = yyvsp[0].name; }
    break;

  case 284:
#line 1006 "ldgram.y"
    { yyval.name = DEFAULT_MEMORY_REGION; }
    break;

  case 285:
#line 1011 "ldgram.y"
    {
		  yyval.section_phdr = NULL;
		}
    break;

  case 286:
#line 1015 "ldgram.y"
    {
		  struct lang_output_section_phdr_list *n;

		  n = ((struct lang_output_section_phdr_list *)
		       xmalloc (sizeof *n));
		  n->name = yyvsp[0].name;
		  n->used = FALSE;
		  n->next = yyvsp[-2].section_phdr;
		  yyval.section_phdr = n;
		}
    break;

  case 288:
#line 1031 "ldgram.y"
    {
			  ldlex_script ();
			  lang_enter_overlay_section (yyvsp[0].name);
			}
    break;

  case 289:
#line 1036 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 290:
#line 1038 "ldgram.y"
    {
			  ldlex_popstate ();
			  lang_leave_overlay_section (yyvsp[0].fill, yyvsp[-1].section_phdr);
			}
    break;

  case 295:
#line 1055 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 296:
#line 1056 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 297:
#line 1058 "ldgram.y"
    {
		  lang_new_phdr (yyvsp[-5].name, yyvsp[-3].etree, yyvsp[-2].phdr.filehdr, yyvsp[-2].phdr.phdrs, yyvsp[-2].phdr.at,
				 yyvsp[-2].phdr.flags);
		}
    break;

  case 298:
#line 1066 "ldgram.y"
    {
		  yyval.etree = yyvsp[0].etree;

		  if (yyvsp[0].etree->type.node_class == etree_name
		      && yyvsp[0].etree->type.node_code == NAME)
		    {
		      const char *s;
		      unsigned int i;
		      static const char * const phdr_types[] =
			{
			  "PT_NULL", "PT_LOAD", "PT_DYNAMIC",
			  "PT_INTERP", "PT_NOTE", "PT_SHLIB",
			  "PT_PHDR", "PT_TLS"
			};

		      s = yyvsp[0].etree->name.name;
		      for (i = 0;
			   i < sizeof phdr_types / sizeof phdr_types[0];
			   i++)
			if (strcmp (s, phdr_types[i]) == 0)
			  {
			    yyval.etree = exp_intop (i);
			    break;
			  }
		      if (i == sizeof phdr_types / sizeof phdr_types[0])
			{
			  if (strcmp (s, "PT_GNU_EH_FRAME") == 0)
			    yyval.etree = exp_intop (0x6474e550);
			  else if (strcmp (s, "PT_GNU_STACK") == 0)
			    yyval.etree = exp_intop (0x6474e551);
			  else
			    {
			      einfo (_("\
%X%P:%S: unknown phdr type `%s' (try integer literal)\n"),
				     s);
			      yyval.etree = exp_intop (0);
			    }
			}
		    }
		}
    break;

  case 299:
#line 1110 "ldgram.y"
    {
		  memset (&yyval.phdr, 0, sizeof (struct phdr_info));
		}
    break;

  case 300:
#line 1114 "ldgram.y"
    {
		  yyval.phdr = yyvsp[0].phdr;
		  if (strcmp (yyvsp[-2].name, "FILEHDR") == 0 && yyvsp[-1].etree == NULL)
		    yyval.phdr.filehdr = TRUE;
		  else if (strcmp (yyvsp[-2].name, "PHDRS") == 0 && yyvsp[-1].etree == NULL)
		    yyval.phdr.phdrs = TRUE;
		  else if (strcmp (yyvsp[-2].name, "FLAGS") == 0 && yyvsp[-1].etree != NULL)
		    yyval.phdr.flags = yyvsp[-1].etree;
		  else
		    einfo (_("%X%P:%S: PHDRS syntax error at `%s'\n"), yyvsp[-2].name);
		}
    break;

  case 301:
#line 1126 "ldgram.y"
    {
		  yyval.phdr = yyvsp[0].phdr;
		  yyval.phdr.at = yyvsp[-2].etree;
		}
    break;

  case 302:
#line 1134 "ldgram.y"
    {
		  yyval.etree = NULL;
		}
    break;

  case 303:
#line 1138 "ldgram.y"
    {
		  yyval.etree = yyvsp[-1].etree;
		}
    break;

  case 304:
#line 1144 "ldgram.y"
    {
		  ldlex_version_file ();
		  PUSH_ERROR (_("dynamic list"));
		}
    break;

  case 305:
#line 1149 "ldgram.y"
    {
		  ldlex_popstate ();
		  POP_ERROR ();
		}
    break;

  case 309:
#line 1166 "ldgram.y"
    {
		  lang_append_dynamic_list (yyvsp[-1].versyms);
		}
    break;

  case 310:
#line 1174 "ldgram.y"
    {
		  ldlex_version_file ();
		  PUSH_ERROR (_("VERSION script"));
		}
    break;

  case 311:
#line 1179 "ldgram.y"
    {
		  ldlex_popstate ();
		  POP_ERROR ();
		}
    break;

  case 312:
#line 1188 "ldgram.y"
    {
		  ldlex_version_script ();
		}
    break;

  case 313:
#line 1192 "ldgram.y"
    {
		  ldlex_popstate ();
		}
    break;

  case 316:
#line 1204 "ldgram.y"
    {
		  lang_register_vers_node (NULL, yyvsp[-2].versnode, NULL);
		}
    break;

  case 317:
#line 1208 "ldgram.y"
    {
		  lang_register_vers_node (yyvsp[-4].name, yyvsp[-2].versnode, NULL);
		}
    break;

  case 318:
#line 1212 "ldgram.y"
    {
		  lang_register_vers_node (yyvsp[-5].name, yyvsp[-3].versnode, yyvsp[-1].deflist);
		}
    break;

  case 319:
#line 1219 "ldgram.y"
    {
		  yyval.deflist = lang_add_vers_depend (NULL, yyvsp[0].name);
		}
    break;

  case 320:
#line 1223 "ldgram.y"
    {
		  yyval.deflist = lang_add_vers_depend (yyvsp[-1].deflist, yyvsp[0].name);
		}
    break;

  case 321:
#line 1230 "ldgram.y"
    {
		  yyval.versnode = lang_new_vers_node (NULL, NULL);
		}
    break;

  case 322:
#line 1234 "ldgram.y"
    {
		  yyval.versnode = lang_new_vers_node (yyvsp[-1].versyms, NULL);
		}
    break;

  case 323:
#line 1238 "ldgram.y"
    {
		  yyval.versnode = lang_new_vers_node (yyvsp[-1].versyms, NULL);
		}
    break;

  case 324:
#line 1242 "ldgram.y"
    {
		  yyval.versnode = lang_new_vers_node (NULL, yyvsp[-1].versyms);
		}
    break;

  case 325:
#line 1246 "ldgram.y"
    {
		  yyval.versnode = lang_new_vers_node (yyvsp[-5].versyms, yyvsp[-1].versyms);
		}
    break;

  case 326:
#line 1253 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (NULL, yyvsp[0].name, ldgram_vers_current_lang, FALSE);
		}
    break;

  case 327:
#line 1257 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (NULL, yyvsp[0].name, ldgram_vers_current_lang, TRUE);
		}
    break;

  case 328:
#line 1261 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (yyvsp[-2].versyms, yyvsp[0].name, ldgram_vers_current_lang, FALSE);
		}
    break;

  case 329:
#line 1265 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (yyvsp[-2].versyms, yyvsp[0].name, ldgram_vers_current_lang, TRUE);
		}
    break;

  case 330:
#line 1269 "ldgram.y"
    {
			  yyval.name = ldgram_vers_current_lang;
			  ldgram_vers_current_lang = yyvsp[-1].name;
			}
    break;

  case 331:
#line 1274 "ldgram.y"
    {
			  struct bfd_elf_version_expr *pat;
			  for (pat = yyvsp[-2].versyms; pat->next != NULL; pat = pat->next);
			  pat->next = yyvsp[-8].versyms;
			  yyval.versyms = yyvsp[-2].versyms;
			  ldgram_vers_current_lang = yyvsp[-3].name;
			}
    break;

  case 332:
#line 1282 "ldgram.y"
    {
			  yyval.name = ldgram_vers_current_lang;
			  ldgram_vers_current_lang = yyvsp[-1].name;
			}
    break;

  case 333:
#line 1287 "ldgram.y"
    {
			  yyval.versyms = yyvsp[-2].versyms;
			  ldgram_vers_current_lang = yyvsp[-3].name;
			}
    break;

  case 334:
#line 1292 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (NULL, "global", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 335:
#line 1296 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (yyvsp[-2].versyms, "global", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 336:
#line 1300 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (NULL, "local", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 337:
#line 1304 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (yyvsp[-2].versyms, "local", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 338:
#line 1308 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (NULL, "extern", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 339:
#line 1312 "ldgram.y"
    {
		  yyval.versyms = lang_new_vers_pattern (yyvsp[-2].versyms, "extern", ldgram_vers_current_lang, FALSE);
		}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 3818 "ldgram.c"

  yyvsp -= yylen;
  yyssp -= yylen;


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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 1322 "ldgram.y"

void
yyerror(arg)
     const char *arg;
{
  if (ldfile_assumed_script)
    einfo (_("%P:%s: file format not recognized; treating as linker script\n"),
	   ldfile_input_filename);
  if (error_index > 0 && error_index < ERROR_NAME_MAX)
     einfo ("%P%F:%S: %s in %s\n", arg, error_names[error_index-1]);
  else
     einfo ("%P%F:%S: %s\n", arg);
}

