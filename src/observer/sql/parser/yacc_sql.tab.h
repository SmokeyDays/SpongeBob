/* A Bison parser, made by GNU Bison 3.7.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_YACC_SQL_TAB_H_INCLUDED
# define YY_YY_YACC_SQL_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    SEMICOLON = 258,               /* SEMICOLON  */
    CREATE = 259,                  /* CREATE  */
    DROP = 260,                    /* DROP  */
    TABLE = 261,                   /* TABLE  */
    TABLES = 262,                  /* TABLES  */
    INDEX = 263,                   /* INDEX  */
    SELECT = 264,                  /* SELECT  */
    DESC = 265,                    /* DESC  */
    SHOW = 266,                    /* SHOW  */
    SYNC = 267,                    /* SYNC  */
    INSERT = 268,                  /* INSERT  */
    DELETE = 269,                  /* DELETE  */
    UPDATE = 270,                  /* UPDATE  */
    LBRACE = 271,                  /* LBRACE  */
    RBRACE = 272,                  /* RBRACE  */
    COMMA = 273,                   /* COMMA  */
    TRX_BEGIN = 274,               /* TRX_BEGIN  */
    TRX_COMMIT = 275,              /* TRX_COMMIT  */
    TRX_ROLLBACK = 276,            /* TRX_ROLLBACK  */
    INT_T = 277,                   /* INT_T  */
    STRING_T = 278,                /* STRING_T  */
    FLOAT_T = 279,                 /* FLOAT_T  */
    HELP = 280,                    /* HELP  */
    EXIT = 281,                    /* EXIT  */
    DOT = 282,                     /* DOT  */
    INTO = 283,                    /* INTO  */
    VALUES = 284,                  /* VALUES  */
    FROM = 285,                    /* FROM  */
    WHERE = 286,                   /* WHERE  */
    AND = 287,                     /* AND  */
    SET = 288,                     /* SET  */
    ON = 289,                      /* ON  */
    LOAD = 290,                    /* LOAD  */
    DATA = 291,                    /* DATA  */
    INFILE = 292,                  /* INFILE  */
    EQ = 293,                      /* EQ  */
    LT = 294,                      /* LT  */
    GT = 295,                      /* GT  */
    LE = 296,                      /* LE  */
    GE = 297,                      /* GE  */
    NE = 298,                      /* NE  */
    DATE_T = 299,                  /* DATE_T  */
    MAX_T = 300,                   /* MAX_T  */
    MIN_T = 301,                   /* MIN_T  */
    AVG_T = 302,                   /* AVG_T  */
    COUNT_T = 303,                 /* COUNT_T  */
    NUMBER = 304,                  /* NUMBER  */
    FLOAT = 305,                   /* FLOAT  */
    ID = 306,                      /* ID  */
    PATH = 307,                    /* PATH  */
    SSS = 308,                     /* SSS  */
    STAR = 309,                    /* STAR  */
    STRING_V = 310,                /* STRING_V  */
    DATE_STRING = 311              /* DATE_STRING  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define SEMICOLON 258
#define CREATE 259
#define DROP 260
#define TABLE 261
#define TABLES 262
#define INDEX 263
#define SELECT 264
#define DESC 265
#define SHOW 266
#define SYNC 267
#define INSERT 268
#define DELETE 269
#define UPDATE 270
#define LBRACE 271
#define RBRACE 272
#define COMMA 273
#define TRX_BEGIN 274
#define TRX_COMMIT 275
#define TRX_ROLLBACK 276
#define INT_T 277
#define STRING_T 278
#define FLOAT_T 279
#define HELP 280
#define EXIT 281
#define DOT 282
#define INTO 283
#define VALUES 284
#define FROM 285
#define WHERE 286
#define AND 287
#define SET 288
#define ON 289
#define LOAD 290
#define DATA 291
#define INFILE 292
#define EQ 293
#define LT 294
#define GT 295
#define LE 296
#define GE 297
#define NE 298
#define DATE_T 299
#define MAX_T 300
#define MIN_T 301
#define AVG_T 302
#define COUNT_T 303
#define NUMBER 304
#define FLOAT 305
#define ID 306
#define PATH 307
#define SSS 308
#define STAR 309
#define STRING_V 310
#define DATE_STRING 311

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 111 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
  char *position;

#line 188 "yacc_sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner);

#endif /* !YY_YY_YACC_SQL_TAB_H_INCLUDED  */
