%{
#ident "$Id$"

/*
 * $Id$
 *
 * Copyright (c) 2003, Kalopa Media Group.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY KALOPA MEDIA GROUP "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA MEDIA GROUP BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ABSTRACT
 *
 * $Log$
 * Revision 1.1  2003/07/28 21:31:58  dtynan
 * First pass at an intelligent front-end for databases.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "dbowint.h"

int		lineno;
int		tlen, tprec;
char		input[512];
char		token[512];
char		*inptr = NULL;
char		*filename;
struct	table	*curtable = NULL;
FILE		*fp;

%}

%union	{
	int		nval;
	char		*sval;
	struct	column	*cval;
	struct	table	*tval;
};

%term	<nval>	VAL
%term	<sval>	IDENT

%term	EMIT TABLE SEARCH FUNCTION PROTO CODE ENDDEF ERROR

%term	KW_AUTOINCR KW_BIGINT KW_BLOB KW_CHAR KW_DECIMAL KW_DATE KW_DATETIME
%term	KW_DOUBLE KW_ENUM KW_FLOAT KW_INT KW_KEY KW_LONGBLOB KW_LONGTEXT
%term	KW_MEDBLOB KW_MEDINT KW_MEDTEXT KW_NATIONAL KW_NOT KW_NULL KW_NUMERIC
%term	KW_PREC KW_PRIMARY KW_REAL KW_SET KW_SMALLINT KW_TEXT KW_TIME KW_TSTAMP
%term	KW_TINYBLOB KW_TINYINT KW_TINYTEXT KW_UNIQUE KW_UNSIGNED KW_VARCHAR
%term	KW_YEAR

%term	',' '(' ')' '{' '}'

%type	<nval>	type optquals optqual
%type	<sval>	ident
%type	<tval>	ntname otname

%%

dbow_file:	  /* Empty statement */
		| dbow_file dbow_stmnt
		;

dbow_stmnt:	  emit_stmnt
		| table_stmnt
		| search_stmnt
		| function_stmnt
		| proto_stmnt
		| code_stmnt
		;

emit_stmnt:	  EMIT ident '{'
		{
			int show = 0;
			struct type *tp;

			if ((tp = findtype($2)) == NULL) {
				yyerror("unknown code generator");
				break;
			}
			if (tp == active)
				show = 1;
			if (active->gensync != NULL)
				active->gensync(filename, lineno + 1, fofp);
			while (lexline() != EOF) {
				if (strcmp(input, "%}") == 0)
					break;
				if (show)
					fprintf(fofp, "%s\n", input);
			}
		}
		| EMIT '{'
		{
			if (active->gensync != NULL)
				active->gensync(filename, lineno + 1, fofp);
			while (lexline() != EOF) {
				if (strcmp(input, "%}") == 0)
					break;
				fprintf(fofp, "%s\n", input);
			}
		}
		;

table_stmnt:	  TABLE ntname '{' table_defs  ENDDEF
		;

search_stmnt:	  SEARCH otname ident
		{
			struct column *cp;

			if ((cp = findcolumn($2, $3)) == NULL) {
				yyerror("cannot find column within table");
				break;
			}
			cp->flags |= FLAG_SEARCH;
		}
		;

function_stmnt:	  FUNCTION
		;

proto_stmnt:	  PROTO
		{
			doproto(filename, lineno);
		}
		;

code_stmnt:	  CODE
		{
			docode(filename, lineno);
		}
		;

ntname:		  ident
		{
			$$ = curtable = newtable($1, 0);
		}
		;

otname:		  ident
		{
			if (($$ = findtable($1)) == NULL)
				yyerror("undefined table");
		}
		;

table_defs:	  table_def
		| table_defs ',' table_def
		;

table_def:	  ident type optquals
		{
			struct column *cp;

			if (findcolumn(curtable, $1) != NULL) {
				yyerror("duplicate column name");
				break;
			}
			cp = newcolumn(curtable, $1, $2, tlen, tprec, $3);
		}
		;

type:		  KW_TINYINT
		{
			$$ = TYPE_TINYINT;
			tlen = tprec = 0;
		}
		| KW_TINYINT '(' VAL ')'
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
		| KW_SMALLINT '(' VAL ')'
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
		| KW_MEDINT '(' VAL ')'
		{
			$$ = TYPE_MEDINT;
			tlen = $3;
			tprec = 0;
		}
		| KW_INT
		{
			$$ = TYPE_INT;
			tlen = tprec = 0;
		}
		| KW_INT '(' VAL ')'
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
		| KW_BIGINT '(' VAL ')'
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
		| KW_FLOAT '(' VAL ',' VAL ')'
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
		| KW_DOUBLE '(' VAL ',' VAL ')'
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
		| KW_DOUBLE KW_PREC '(' VAL ',' VAL ')'
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
		| KW_REAL '(' VAL ',' VAL ')'
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
		| KW_DECIMAL '(' VAL ')'
		{
			$$ = TYPE_NUMERIC;
			tlen = $3;
			tprec = 0;
		}
		| KW_DECIMAL '(' VAL ',' VAL ')'
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
		| KW_NUMERIC '(' VAL ',' VAL ')'
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
		| KW_TSTAMP '(' VAL ')'
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
		| KW_YEAR '(' VAL ')'
		{
			$$ = TYPE_YEAR;
			tlen = $3;
			tprec = 0;
		}
		| KW_CHAR '(' VAL ')'
		{
			$$ = TYPE_CHAR;
			tlen = $3;
			tprec = 0;
		}
		| KW_NATIONAL KW_CHAR '(' VAL ')'
		{
			$$ = TYPE_CHAR;
			tlen = $4;
			tprec = 0;
		}
		| KW_VARCHAR '(' VAL ')'
		{
			$$ = TYPE_VARCHAR;
			tlen = $3;
			tprec = 0;
		}
		| KW_NATIONAL KW_VARCHAR '(' VAL ')'
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
%%

/*
 * Keyword parsing structure...
 */
struct	keyword	{
	char	*name;
	int	value;
} keywords[] = {
	"auto_increment",	KW_AUTOINCR,
	"bigint",		KW_BIGINT,
	"blob",			KW_BLOB,
	"char",			KW_CHAR,
	"decimal",		KW_DECIMAL,
	"date",			KW_DATE,
	"datetime",		KW_DATETIME,
	"double",		KW_DOUBLE,
	"enum",			KW_ENUM,
	"float",		KW_FLOAT,
	"int",			KW_INT,
	"integer",		KW_INT,
	"key",			KW_KEY,
	"longblob",		KW_LONGBLOB,
	"longtext",		KW_LONGTEXT,
	"mediumblob",		KW_MEDBLOB,
	"mediumint",		KW_MEDINT,
	"mediumtext",		KW_MEDTEXT,
	"national",		KW_NATIONAL,
	"not",			KW_NOT,
	"null",			KW_NULL,
	"numeric",		KW_NUMERIC,
	"precision",		KW_PREC,
	"primary",		KW_PRIMARY,
	"real",			KW_REAL,
	"set",			KW_SET,
	"smallint",		KW_SMALLINT,
	"text",			KW_TEXT,
	"time",			KW_TIME,
	"timestamp",		KW_TSTAMP,
	"tinyblob",		KW_TINYBLOB,
	"tinyint",		KW_TINYINT,
	"tinytext",		KW_TINYTEXT,
	"unique",		KW_UNIQUE,
	"unsigned",		KW_UNSIGNED,
	"varchar",		KW_VARCHAR,
	"year",			KW_YEAR,
	NULL,			0
};

/*
 *
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
	return(0);
}

/*
 *
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
 *
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
	return(0);
}

/*
 *
 */
char *
lexword()
{
	char *cp;

	if (!isalpha(*inptr) && *inptr != '_')
		return(NULL);
	cp = token;
	*cp++ = *inptr++;
	while (isalnum(*inptr) || *inptr == '_')
		*cp++ = *inptr++;
	*cp = '\0';
	return(token);
}

/*
 *
 */
int
yylex()
{
	int atstart = 0;
	struct keyword *kp;
	char *cp;

	while (inptr != NULL && isspace(*inptr))
		inptr++;
	while (inptr == NULL || *inptr == '\0') {
		if (lexline() == EOF)
			return(EOF);
		for (cp = input; isspace(*cp); cp++)
			;
		if (*cp == '#' || *cp == '\0')
			continue;
		inptr = cp;
		atstart = 1;
	}
	if (atstart && *inptr == '%') {
		inptr++;
		while (isspace(*inptr))
			inptr++;
		if (*inptr == '}') {
			inptr = NULL;
			return(ENDDEF);
		}
		if ((cp = lexword()) == NULL)
			return(ERROR);
		if (strcmp(cp, "code") == 0)
			return(CODE);
		if (strcmp(cp, "emit") == 0)
			return(EMIT);
		if (strcmp(cp, "function") == 0)
			return(FUNCTION);
		if (strcmp(cp, "proto") == 0)
			return(PROTO);
		if (strcmp(cp, "search") == 0)
			return(SEARCH);
		if (strcmp(cp, "table") == 0)
			return(TABLE);
		return(ERROR);
	}
	switch (*inptr) {
	case ',':
	case '(':
	case ')':
	case '{':
	case '}':
		return(*inptr++);

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		yylval.nval = 0;
		while (isdigit(*inptr))
			yylval.nval = yylval.nval * 10 + *inptr++ - '0';
		return(VAL);
	}
	if (lexword() != NULL) {
		for (kp = keywords; kp->name != NULL; kp++)
			if (strcasecmp(token, kp->name) == 0)
				return(kp->value);
		yylval.sval = strdup(token);
		return(IDENT);
	}
	return(ERROR);
}

/*
 *
 */
void
yyerror(char *msg)
{
	if (filename != NULL && lineno > 0)
		fprintf(stderr, "\"%s\", line %d: ", filename, lineno);
	fprintf(stderr, "%s.\n", msg);
	nerrors++;
}
