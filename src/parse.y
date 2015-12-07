%{
/*
 * Copyright (c) 2003, Kalopa Media Limited.
 * Copyright (c) 2015, Kalopa Research.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * It is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this product; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139,
 * USA.
 *
 * THIS SOFTWARE IS PROVIDED BY KALOPA RESEARCH "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA
 * RESEARCH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "dbowint.h"
#include "dbow.h"

#define MAXLINELEN	512
#define MAXTOKENLEN	64

int		lineno;
int		tlen, tprec;
char		input[MAXLINELEN+2];
char		*inptr = NULL;
char		*filename;
struct	table	*curtable = NULL;
struct	func	*curfunc = NULL;
FILE		*fp;

%}

%union	{
	int		nval;
	char		*ival;
	char		*sval;
	struct	column	*cval;
	struct	table	*tval;
	struct	arg	*aval;
	struct	set	*setv;
	struct	func	*fval;
};

%term	<nval>	VAL
%term	<sval>	STRING
%term	<ival>	IDENT

%term	PCENT PTR ERROR
%term	KW_ALIAS KW_ARGS KW_BY KW_CODE KW_DELETE KW_DUMP KW_EMIT KW_FUNCTION
%term	KW_INSERT KW_JOIN KW_LIMIT KW_NAME KW_OFFSET KW_ORDER KW_PROTO
%term	KW_RETURNS KW_SEARCH KW_SET KW_SQL KW_TABLE KW_TYPE KW_UPDATE KW_WHERE

%term	KW_AUTOINCR KW_BIGINT KW_BLOB KW_CHAR KW_DECIMAL KW_DATE KW_DATETIME
%term	KW_DOUBLE KW_ENUM KW_FLOAT KW_INT KW_KEY KW_LONGBLOB KW_LONGTEXT
%term	KW_MEDBLOB KW_MEDINT KW_MEDTEXT KW_NATIONAL KW_NOT KW_NULL KW_NUMERIC
%term	KW_PREC KW_PRIMARY KW_REAL KW_SMALLINT KW_TEXT KW_TIME KW_TSTAMP
%term	KW_TINYBLOB KW_TINYINT KW_TINYTEXT KW_UNIQUE KW_UNSIGNED KW_VARCHAR
%term	KW_YEAR

%term	'=' ';' ',' '.' '(' ')' '{' '}' '/'

%type	<nval>	number class opttype optquals optqual func_type
%type	<sval>	string
%type	<ival>	ident
%type	<tval>	ntname
%type	<fval>	nfname
%type	<aval>	arg arg_list
%type	<setv>	set set_list

%%

dbow_file:	  /* Empty statement */
		| dbow_file dbow_stmnt
		;

dbow_stmnt:	  emit_stmnt
		| table_stmnt
		| function_stmnt
		| dump_stmnt
		;

emit_stmnt:	  PCENT opttype KW_CODE '{'
		{
			linesync(filename, lineno + 1, fofp);
			while (lexline() != EOF) {
				if (strcmp(input, "%}") == 0)
					break;
				if ($2)
					fprintf(fofp, "%s\n", input);
			}
		}
		| PCENT opttype KW_PROTO '{'
		{
			linesync(filename, lineno + 1, hofp);
			while (lexline() != EOF) {
				if (strcmp(input, "%}") == 0)
					break;
				if ($2)
					fprintf(hofp, "%s\n", input);
			}
		}
		| PCENT opttype KW_EMIT '{'
		{
			int show = $2;

			if (tofp == NULL && gentmpf() < 0)
				show = 0;
			else
				linesync(filename, lineno + 1, tofp);
			while (lexline() != EOF) {
				if (strcmp(input, "%}") == 0)
					break;
				if (show)
					fprintf(tofp, "%s\n", input);
			}
		}
		;

table_stmnt:	  PCENT KW_TABLE ntname '{' table_defs  PCENT '}'
		{
			int sawpk = 0;
			struct column *cp;

			for (cp = $3->chead; cp != NULL; cp = cp->next)
				if (cp->flags & FLAG_PRIKEY) {
					sawpk = 1;
					break;
			}
			if (!sawpk)
				yyerror("no primary key defined in table");
		}
		;

function_stmnt:	  PCENT opttype KW_FUNCTION nfname '{' func_defs PCENT '}'
		| PCENT opttype KW_FUNCTION nfname func_def
		| insert_stmnt
		| delete_stmnt
		| search_stmnt
		| update_stmnt
		;

insert_stmnt:	  PCENT opttype KW_INSERT nfname optsemi
		{
			if ($2)
				curfunc->type = DBOW_INSERT;
		}
		| PCENT opttype KW_INSERT nfname ident optsemi
		{
			if ($2) {
				curfunc->type = DBOW_INSERT;
				curfunc->name = $5;
			}
		}
		;

delete_stmnt:	  PCENT opttype KW_DELETE nfname ident optsemi
		{
			if ($2) {
				curfunc->type = DBOW_DELETE;
				curfunc->pkey = findcolumn(curfunc->table, $5);
			}
		}
		| PCENT opttype KW_DELETE nfname ident ident optsemi
		{
			if ($2) {
				curfunc->type = DBOW_DELETE;
				curfunc->pkey = findcolumn(curfunc->table, $5);
				curfunc->name = $6;
			}
		}
		;

search_stmnt:	  PCENT opttype KW_SEARCH nfname ident optsemi
		{
			if ($2) {
				curfunc->type = DBOW_SEARCH;
				curfunc->pkey = findcolumn(curfunc->table, $5);
			}
		}
		| PCENT opttype KW_SEARCH nfname ident ident optsemi
		{
			if ($2) {
				curfunc->type = DBOW_SEARCH;
				curfunc->pkey = findcolumn(curfunc->table, $5);
				curfunc->name = $6;
			}
		}
		;

update_stmnt:	  PCENT opttype KW_UPDATE nfname ident optsemi
		{
			if ($2) {
				curfunc->type = DBOW_UPDATE;
				curfunc->pkey = findcolumn(curfunc->table, $5);
			}
		}
		| PCENT opttype KW_UPDATE nfname ident ident optsemi
		{
			if ($2) {
				curfunc->type = DBOW_UPDATE;
				curfunc->pkey = findcolumn(curfunc->table, $5);
				curfunc->name = $6;
			}
		}
		;

dump_stmnt:	  PCENT opttype KW_DUMP nfname optsemi
		{
			if ($2) {
				curfunc->type = DBOW_OTHER;
				curfunc->flags |= DBOW_DUMP;
			}
		}
		;

opttype:	  /* Empty */
		{
			$$ = 1;
		}
		| KW_TYPE ident
		{
			struct type *tp;

			if ((tp = findtype($2)) == NULL) {
				yyerror("unknown code generator type");
				break;
			}
			$$ = (tp == active) ? 1 : 0;
		}
		;

optsemi:	  /* Empty */
		| ';'
		;

ntname:		  ident
		{
			$$ = curtable = newtable($1, 0);
			free($1);
		}
		;

nfname:		  ident
		{
			$$ = curfunc = newfunction($1, 0);
			free($1);
		}
		;

table_defs:	  table_def ';'
		| table_defs table_def ';'
		;


func_defs:	  func_def
		| func_defs func_def
		;

table_def:	  ident class optquals
		{
			struct column *cp;

			if (findcolumn(curtable, $1) != NULL) {
				yyerror("duplicate column name");
				break;
			}
			cp = newcolumn(curtable, $1, $2, tlen, tprec, $3);
		}
		;

func_def:	  KW_TYPE func_type ';'
		{
			if (curfunc != NULL)
				curfunc->type = $2;
		}
		| KW_TYPE func_type KW_BY ident ';'
		{
			struct column *cp;

			if (curfunc == NULL)
				break;
			if ((cp = findcolumn(curfunc->table, $4)) == NULL) {
				yyerror("invalid column name for table");
				break;
			}
			curfunc->type = $2;
			curfunc->pkey = cp;
		}
		| KW_NAME ident ';'
		{
			if (curfunc != NULL)
				curfunc->name = $2;
		}
		| KW_SQL string ';'
		{
			if (curfunc != NULL)
				curfunc->query = strdup($2);
		}
		| KW_WHERE string ';'
		{
			if (curfunc != NULL)
				curfunc->where = strdup($2);
		}
		| KW_ORDER string ';'
		{
			if (curfunc != NULL)
				curfunc->order = strdup($2);
		}
		| KW_ORDER KW_BY string ';'
		{
			if (curfunc != NULL)
				curfunc->order = strdup($3);
		}
		| KW_LIMIT number ';'
		{
			if (curfunc != NULL)
				curfunc->limit = $2;
		}
		| KW_OFFSET number ';'
		{
			if (curfunc != NULL)
				curfunc->offset = $2;
		}
		| KW_ARGS arg_list ';'
		{
			if (curfunc != NULL)
				curfunc->args = $2;
		}
		| KW_ALIAS ident ';'
		{
			if (curfunc != NULL)
				curfunc->alias = $2;
		}
		| KW_JOIN ident ';'
		{
			struct join *jp = newjoin($2, NULL);

			if (jp != NULL && jp->table == NULL)
				yyerror("invalid table for join");
			else if (curfunc != NULL) {
				jp->next = curfunc->joins;
				curfunc->joins = jp;
			}
		}
		| KW_JOIN ident KW_ALIAS ident ';'
		{
			struct join *jp = newjoin($2, $4);

			if (jp != NULL && jp->table == NULL)
				yyerror("invalid table for join");
			else if (curfunc != NULL) {
				jp->next = curfunc->joins;
				curfunc->joins = jp;
			}
		}
		| KW_SET set_list ';'
		{
		}
		| KW_RETURNS arg_list ';'
		;

func_type:	  KW_INSERT
		{
			$$ = DBOW_INSERT;
		}
		| KW_SEARCH
		{
			$$ = DBOW_SEARCH;
		}
		| KW_UPDATE
		{
			$$ = DBOW_UPDATE;
		}
		| KW_DELETE
		{
			$$ = DBOW_DELETE;
		}
		;

set_list:	  set
		| set_list ',' set
		{
			struct set *sp = $1;

			while (sp->next != NULL)
				sp = sp->next;
			sp->next = $3;
			$$ = $1;
		}
		;

set:		  arg '=' arg
		{
			$$ = newset($1, $3);
		}
		;

arg_list:	  arg
		| arg_list ',' arg
		{
			struct arg *ap = $1;

			while (ap->next != NULL)
				ap = ap->next;
			ap->next = $3;
			$$ = $1;
		}
		;

arg:		  ident
		{
			if (curfunc == NULL)
				break;
			$$ = newarg(curfunc->table, $1);
			if ($$->column == NULL)
				yyerror("invalid column name");
		}
		| ident '.' ident
		{
			struct table *tp;

			if (curfunc == NULL)
				break;
			if ((tp = findaliastable(curfunc, $1)) == NULL)
				yyerror("invalid table for argument");
			else
				$$ = newarg(tp, $3);
		}
		| ident PTR ident
		{
			if (curfunc == NULL)
				break;
			$$ = newarg(curfunc->table, $3);
			$$->handle = $1;
		}
		;

class:		  KW_TINYINT
		{
			$$ = TYPE_TINYINT;
			tlen = tprec = 0;
		}
		| KW_TINYINT '(' number ')'
		{
			$$ = TYPE_TINYINT;
			tlen = $3;
			tprec = 0;
		}
		| KW_SMALLINT
		{
			$$ = TYPE_SMALLINT;
			tlen = tprec = 0;
		}
		| KW_SMALLINT '(' number ')'
		{
			$$ = TYPE_SMALLINT;
			tlen = $3;
			tprec = 0;
		}
		| KW_MEDINT
		{
			$$ = TYPE_MEDINT;
			tlen = tprec = 0;
		}
		| KW_MEDINT '(' number ')'
		{
			$$ = TYPE_MEDINT;
			tlen = $3;
			tprec = 0;
		}
		| KW_INT
		{
			$$ = TYPE_INT;
			tlen = 11;
			tprec = 0;
		}
		| KW_INT '(' number ')'
		{
			$$ = TYPE_INT;
			tlen = $3;
			tprec = 0;
		}
		| KW_BIGINT
		{
			$$ = TYPE_BIGINT;
			tlen = tprec = 0;
		}
		| KW_BIGINT '(' number ')'
		{
			$$ = TYPE_BIGINT;
			tlen = $3;
			tprec = 0;
		}
		| KW_FLOAT
		{
			$$ = TYPE_FLOAT;
			tlen = tprec = 0;
		}
		| KW_FLOAT '(' number ',' number ')'
		{
			$$ = TYPE_FLOAT;
			tlen = $3;
			tprec = $5;
		}
		| KW_DOUBLE
		{
			$$ = TYPE_DOUBLE;
			tlen = tprec = 0;
		}
		| KW_DOUBLE '(' number ',' number ')'
		{
			$$ = TYPE_DOUBLE;
			tlen = $3;
			tprec = $5;
		}
		| KW_DOUBLE KW_PREC
		{
			$$ = TYPE_DOUBLE;
			tlen = tprec = 0;
		}
		| KW_DOUBLE KW_PREC '(' number ',' number ')'
		{
			$$ = TYPE_DOUBLE;
			tlen = $4;
			tprec = $6;
		}
		| KW_REAL
		{
			$$ = TYPE_DOUBLE;
			tlen = tprec = 0;
		}
		| KW_REAL '(' number ',' number ')'
		{
			$$ = TYPE_DOUBLE;
			tlen = $3;
			tprec = $5;
		}
		| KW_DECIMAL
		{
			$$ = TYPE_NUMERIC;
			tlen = tprec = 0;
		}
		| KW_DECIMAL '(' number ')'
		{
			$$ = TYPE_NUMERIC;
			tlen = $3;
			tprec = 0;
		}
		| KW_DECIMAL '(' number ',' number ')'
		{
			$$ = TYPE_NUMERIC;
			tlen = $3;
			tprec = $5;
		}
		| KW_NUMERIC
		{
			$$ = TYPE_NUMERIC;
			tlen = tprec = 0;
		}
		| KW_NUMERIC '(' number ',' number ')'
		{
			$$ = TYPE_NUMERIC;
			tlen = $3;
			tprec = $5;
		}
		| KW_DATE
		{
			$$ = TYPE_DATE;
			tlen = tprec = 0;
		}
		| KW_TIME
		{
			$$ = TYPE_TIME;
			tlen = tprec = 0;
		}
		| KW_DATETIME
		{
			$$ = TYPE_DATETIME;
			tlen = tprec = 0;
		}
		| KW_TSTAMP
		{
			$$ = TYPE_TSTAMP;
			tlen = tprec = 0;
		}
		| KW_TSTAMP '(' number ')'
		{
			$$ = TYPE_TSTAMP;
			tlen = $3;
			tprec = 0;
		}
		| KW_YEAR
		{
			$$ = TYPE_YEAR;
			tlen = tprec = 0;
		}
		| KW_YEAR '(' number ')'
		{
			$$ = TYPE_YEAR;
			tlen = $3;
			tprec = 0;
		}
		| KW_CHAR '(' number ')'
		{
			$$ = TYPE_CHAR;
			tlen = $3;
			tprec = 0;
		}
		| KW_NATIONAL KW_CHAR '(' number ')'
		{
			$$ = TYPE_CHAR;
			tlen = $4;
			tprec = 0;
		}
		| KW_VARCHAR '(' number ')'
		{
			$$ = TYPE_VARCHAR;
			tlen = $3;
			tprec = 0;
		}
		| KW_NATIONAL KW_VARCHAR '(' number ')'
		{
			$$ = TYPE_VARCHAR;
			tlen = $4;
			tprec = 0;
		}
		| KW_TINYBLOB
		{
			$$ = TYPE_TINYBLOB;
			tlen = tprec = 0;
		}
		| KW_TINYTEXT
		{
			$$ = TYPE_TINYTEXT;
			tlen = tprec = 0;
		}
		| KW_BLOB
		{
			$$ = TYPE_BLOB;
			tlen = tprec = 0;
		}
		| KW_TEXT
		{
			$$ = TYPE_TEXT;
			tlen = tprec = 0;
		}
		| KW_MEDBLOB
		{
			$$ = TYPE_MEDBLOB;
			tlen = tprec = 0;
		}
		| KW_MEDTEXT
		{
			$$ = TYPE_MEDTEXT;
			tlen = tprec = 0;
		}
		| KW_LONGBLOB
		{
			$$ = TYPE_LONGBLOB;
			tlen = tprec = 0;
		}
		| KW_LONGTEXT
		{
			$$ = TYPE_LONGTEXT;
			tlen = tprec = 0;
		}
		| KW_ENUM '(' ')'
		{
			yyerror("SQL 'enum' types are not supported in this version");
		}
		| KW_SET '(' ')'
		{
			yyerror("SQL 'set' types are not supported in this version");
		}
		;

optquals:	  /* Empty */
		{
			$$ = 0;
		}
		| optquals optqual
		{
			$$ = $1 | $2;
		}
		;

optqual:	  KW_UNSIGNED { $$ = FLAG_UNSIGNED; }
		| KW_NOT KW_NULL { $$ = FLAG_NOTNULL; }
		| KW_PRIMARY KW_KEY { $$ = FLAG_PRIKEY; }
		| KW_UNIQUE { $$ = FLAG_UNIQUE; }
		| KW_AUTOINCR { $$ = FLAG_AUTOINC; }
		;

ident:		  IDENT
		;

string:		  STRING
		;

number:		  VAL
		;
%%

/*
 * Keyword parsing structure...
 */
struct	keyword	{
	char	*name;
	int	value;
} keywords[] = {
	{"alias",		KW_ALIAS},
	{"args",		KW_ARGS},
	{"auto_increment",	KW_AUTOINCR},
	{"bigint",		KW_BIGINT},
	{"blob",		KW_BLOB},
	{"by",			KW_BY},
	{"char",		KW_CHAR},
	{"code",		KW_CODE},
	{"decimal",		KW_DECIMAL},
	{"delete",		KW_DELETE},
	{"date",		KW_DATE},
	{"datetime",		KW_DATETIME},
	{"double",		KW_DOUBLE},
	{"dump",		KW_DUMP},
	{"emit",		KW_EMIT},
	{"enum",		KW_ENUM},
	{"float",		KW_FLOAT},
	{"function",		KW_FUNCTION},
	{"insert",		KW_INSERT},
	{"int",			KW_INT},
	{"integer",		KW_INT},
	{"join",		KW_JOIN},
	{"key",			KW_KEY},
	{"limit",		KW_LIMIT},
	{"longblob",		KW_LONGBLOB},
	{"longtext",		KW_LONGTEXT},
	{"mediumblob",		KW_MEDBLOB},
	{"mediumint",		KW_MEDINT},
	{"mediumtext",		KW_MEDTEXT},
	{"national",		KW_NATIONAL},
	{"name",		KW_NAME},
	{"not",			KW_NOT},
	{"null",		KW_NULL},
	{"numeric",		KW_NUMERIC},
	{"offset",		KW_OFFSET},
	{"order",		KW_ORDER},
	{"precision",		KW_PREC},
	{"primary",		KW_PRIMARY},
	{"proto",		KW_PROTO},
	{"real",		KW_REAL},
	{"returns",		KW_RETURNS},
	{"search",		KW_SEARCH},
	{"set",			KW_SET},
	{"sql",			KW_SQL},
	{"smallint",		KW_SMALLINT},
	{"table",		KW_TABLE},
	{"text",		KW_TEXT},
	{"time",		KW_TIME},
	{"timestamp",		KW_TSTAMP},
	{"tinyblob",		KW_TINYBLOB},
	{"tinyint",		KW_TINYINT},
	{"tinytext",		KW_TINYTEXT},
	{"type",		KW_TYPE},
	{"update",		KW_UPDATE},
	{"unique",		KW_UNIQUE},
	{"unsigned",		KW_UNSIGNED},
	{"varchar",		KW_VARCHAR},
	{"where",		KW_WHERE},
	{"year",		KW_YEAR},
	{NULL,			0}
};

/*
 * Open a source file for subsequent parsing.  Note that the
 * file descriptor, file name and source line number are
 * globals.  It's too hard to pass anything into YACC and
 * anyway, who cares?
 */
int
lexopen(char *fname)
{
	lexclose();
	if (fname == NULL || strcmp(fname, "-") == 0) {
		fp = stdin;
		filename = strdup("(stdin)");
	} else {
		if ((fp = fopen(fname, "r")) == NULL)
			return(-1);
		filename = strdup(fname);
	}
	lineno = nerrors = 0;
	inptr = NULL;
	return(0);
}

/*
 * Close an open source file (if there is one).
 */
void
lexclose()
{
	if (fp != NULL)
		fclose(fp);
	if (filename != NULL)
		free(filename);
	filename = NULL;
}

/*
 * Retrieve a single line from the source file, strip the CR/NL from
 * the end where applicable, and keep an eye on the line numbering.
 */
int
lexline()
{
	char *cp;

	inptr = NULL;
	if (fp == NULL || fgets(input, sizeof(input) - 2, fp) == NULL)
		return(EOF);
	lineno++;
	if ((cp = strpbrk(input, "\r\n")) != NULL)
		*cp = '\0';
#ifdef YYDEBUG
	if (yydebug)
		printf("Line %d: [%s]\n", lineno, input);
#endif
	return(0);
}

/*
 * Return a single lexical token to the parser.  This is done by
 * keeping a pointer into the source line and when the pointer
 * falls off the end, fetch a new line.  Then return the next
 * lexical token in the line.  Some special stuff happens here
 * because some tokens (such as PCENT) begin at the start of a
 * line only.  Anywhere else, the token '%' is returned.
 */
int
yylex()
{
	struct keyword *kp;
	char *cp, token[MAXTOKENLEN+2];

	/*
	 * Skip any leading whitespace.
	 */
	while (inptr != NULL && isspace(*inptr))
		inptr++;
	/*
	 * If we don't have a line in play, then keep reading from the
	 * source file until we have a decent input line or we reach
	 * the end of the file.
	 */
	while (inptr == NULL || *inptr == '\0') {
		if (lexline() == EOF)
			return(EOF);
		/*
		 * Trim leading whitespace on the line.
		 */
		for (cp = input; isspace(*cp); cp++)
			;
		/*
		 * A comment uses a '#' at the start of the line.  If
		 * we see a comment character or a blank line, then
		 * try again.
		 */
		if (*cp == '#' || *cp == '\0')
			continue;
		/*
		 * It's a keeper.  Check for the presence of a
		 * percent at the start of the line.
		 */
		inptr = cp;
		if (*inptr == '%') {
			inptr++;
			return(PCENT);
		}
	}
	/*
	 * We have a valid line with non-whitespace in front.  See
	 * if we can do this the easy way...
	 */
	switch (*inptr) {
	case '=':
	case ';':
	case ',':
	case '.':
	case '(':
	case ')':
	case '{':
	case '}':
	case '/':
		/*
		 * A special character - just return it as-is.
		 */
		return(*inptr++);

	case '-':
		/*
		 * Oops!  We might have a pointer specifier.
		 */
		inptr++;
		if (*inptr == '>') {
			inptr++;
			return(PTR);
		}
		return('-');

	case '"':
		/*
		 * Seems to be a character string.  Store it up until
		 * we reach the end of it.
		 */
		yylval.sval = ++inptr;
		while (*inptr != '\0' && *inptr != '"') {
			if (*inptr == '\\') {
				inptr++;
				if (*inptr == '\0')
					break;
			}
			inptr++;
		}
		if (*inptr != '\0')
			*inptr++ = '\0';
		return(STRING);

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		/*
		 * A numeric value.  Keep eating digits until we
		 * run out.
		 */
		yylval.nval = 0;
		while (isdigit(*inptr))
			yylval.nval = yylval.nval * 10 + *inptr++ - '0';
		return(VAL);
	}
	/*
	 * OK, at this point it better be some sort of identifier.
	 * An identifier begins with an alpha character or underscore.
	 * If we see anything else in the stream, it's an error.
	 */
	if (!isalpha(*inptr) && *inptr != '_')
		return(ERROR);
	/*
	 * Stuff all the characters we see until some delimiter
	 * into a temporary buffer first.
	 */
	cp = token;
	*cp++ = *inptr++;
	while (cp < &token[MAXTOKENLEN]) {
		if (!isalnum(*inptr) && *inptr != '_')
			break;
		*cp++ = *inptr++;
	}
	*cp = '\0';
	/*
	 * Look through the official list of keywords to see if
	 * we can find this particular entry.
	 */
	for (kp = keywords; kp->name != NULL; kp++)
		if (strcasecmp(token, kp->name) == 0)
			return(kp->value);
	/*
	 * No?  It must be a symbol or identifier or something.
	 * Return it as a string identifier.
	 */
	yylval.ival = strdup(token);
	return(IDENT);
}

/*
 * Generic YACC error function.  Mostly YACC returns errors of the
 * form "syntax error" which isn't useful.  At least we can
 * report the line and filename, which is a small help.
 */
void
yyerror(const char *msg)
{
	if (filename != NULL && lineno > 0)
		printf("\"%s\", line %d: ", filename, lineno);
	printf("%s.\n", msg);
	nerrors++;
}
