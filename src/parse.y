%{
#ident "$Id$"

/*
 * $Id$
 *
 * Copyright (c) 2003, Kalopa Media Limited.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY KALOPA MEDIA LIMITED "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA MEDIA LIMITED BE
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
 * Revision 1.2  2003/11/17 13:15:20  dtynan
 * Various changes to fix errors in the back-end code.
 *
 * Revision 1.1  2003/10/14 13:00:28  dtynan
 * Major revision of the DBOW code to use M4 as a back-end instead of
 * hard-coding the output.
 *
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

%term	PCENT ERROR
%term	KW_CODE KW_DELETE KW_DUMP KW_EMIT KW_FUNCTION KW_INSERT
%term	KW_PROTO KW_SEARCH KW_TABLE KW_TYPE KW_UPDATE

%term	KW_AUTOINCR KW_BIGINT KW_BLOB KW_CHAR KW_DECIMAL KW_DATE KW_DATETIME
%term	KW_DOUBLE KW_ENUM KW_FLOAT KW_INT KW_KEY KW_LONGBLOB KW_LONGTEXT
%term	KW_MEDBLOB KW_MEDINT KW_MEDTEXT KW_NATIONAL KW_NOT KW_NULL KW_NUMERIC
%term	KW_PREC KW_PRIMARY KW_REAL KW_SET KW_SMALLINT KW_TEXT KW_TIME KW_TSTAMP
%term	KW_TINYBLOB KW_TINYINT KW_TINYTEXT KW_UNIQUE KW_UNSIGNED KW_VARCHAR
%term	KW_YEAR

%term	',' '(' ')' '{' '}'

%type	<nval>	class opttype optquals optqual
%type	<sval>	ident
%type	<tval>	ntname otname

%%

dbow_file:	  /* Empty statement */
		| dbow_file dbow_stmnt
		;

dbow_stmnt:	  emit_stmnt
		| table_stmnt
		| insert_stmnt
		| delete_stmnt
		| search_stmnt
		| update_stmnt
		| function_stmnt
		| dump_stmnt
		| proto_stmnt
		| code_stmnt
		;

emit_stmnt:	  PCENT opttype KW_EMIT '{'
		{
			int show = $2;

			linesync(filename, lineno + 1, fofp);
			while (lexline() != EOF) {
				if (strcmp(input, "%}") == 0)
					break;
				if (show)
					fprintf(fofp, "%s\n", input);
			}
		}
		;

table_stmnt:	  PCENT KW_TABLE ntname '{' table_defs  PCENT '}'
		{
			int sawpk = 0;
			struct column *cp;

			for (cp = $3->chead; cp != NULL; cp = cp->next)
				if (cp->flags & FLAG_PRIKEY)
					sawpk = 1;
			if (!sawpk)
				yyerror("no primary key defined in table");
		}
		;

insert_stmnt:	  PCENT opttype KW_INSERT otname
		{
			if ($2)
				genfuncname($4, NULL, NULL, DBOW_INSERT);
		}
		| PCENT opttype KW_INSERT otname ident
		{
			if ($2)
				genfuncname($4, NULL, $5, DBOW_INSERT);
		}
		;

delete_stmnt:	  PCENT opttype KW_DELETE otname ident
		{
			if ($2)
				genfuncname($4, $5, NULL, DBOW_DELETE);
		}
		| PCENT opttype KW_DELETE otname ident ident
		{
			if ($2)
				genfuncname($4, $5, $6, DBOW_DELETE);
		}
		;

search_stmnt:	  PCENT opttype KW_SEARCH otname ident
		{
			if ($2)
				genfuncname($4, $5, NULL, DBOW_SEARCH);
		}
		| PCENT opttype KW_SEARCH otname ident ident
		{
			if ($2)
				genfuncname($4, $5, $6, DBOW_SEARCH);
		}
		;

update_stmnt:	  PCENT opttype KW_UPDATE otname ident
		{
			if ($2)
				genfuncname($4, $5, NULL, DBOW_UPDATE);
		}
		| PCENT opttype KW_UPDATE otname ident ident
		{
			if ($2)
				genfuncname($4, $5, $6, DBOW_UPDATE);
		}
		;

function_stmnt:	  PCENT opttype KW_FUNCTION otname '{' PCENT '}'
		{
			if (!$2)
				break;
#ifdef NOTYET
%type C function {
	type	search
	table	phonedir;
	name	db_phonefindbyid;
	column	phone_id;
};
#endif
		}
		;

dump_stmnt:	  PCENT opttype KW_DUMP otname
		{
			if ($2)
				$4->flags |= FLAG_DUMP;
		}
		;

proto_stmnt:	  PCENT KW_PROTO
		{
			doproto(filename, lineno);
		}
		;

code_stmnt:	  PCENT KW_CODE
		{
			docode(filename, lineno);
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

class:		  KW_TINYINT
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
	{"auto_increment",	KW_AUTOINCR},
	{"bigint",		KW_BIGINT},
	{"blob",		KW_BLOB},
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
	{"key",			KW_KEY},
	{"longblob",		KW_LONGBLOB},
	{"longtext",		KW_LONGTEXT},
	{"mediumblob",		KW_MEDBLOB},
	{"mediumint",		KW_MEDINT},
	{"mediumtext",		KW_MEDTEXT},
	{"national",		KW_NATIONAL},
	{"not",			KW_NOT},
	{"null",		KW_NULL},
	{"numeric",		KW_NUMERIC},
	{"precision",		KW_PREC},
	{"primary",		KW_PRIMARY},
	{"proto",		KW_PROTO},
	{"real",		KW_REAL},
	{"search",		KW_SEARCH},
	{"set",			KW_SET},
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
	case ',':
	case '(':
	case ')':
	case '{':
	case '}':
		/*
		 * A special character - just return it as-is.
		 */
		return(*inptr++);

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
	yylval.sval = strdup(token);
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
		fprintf(stderr, "\"%s\", line %d: ", filename, lineno);
	fprintf(stderr, "%s.\n", msg);
	nerrors++;
}
