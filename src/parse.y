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
 * Revision 1.7  2004/06/28 15:46:42  dtynan
 * Call the function name 'fname' rather than 'name' which collides too much.
 *
 * Revision 1.6  2004/06/25 14:57:23  dtynan
 * Fixed a bug in the C template where forloops weren't working properly.
 * Added a RELEASE file, first pass at a man-page, and the basic hooks
 * for dealing with th %function block.
 *
 * Revision 1.5  2004/05/18 11:18:48  dtynan
 * Deprecated the use of %proto and %code statements.  Also added new
 * keywords which will immediately emit the following block either to
 * the include file or to the code file respectively.
 *
 * Revision 1.4  2004/04/30 12:12:25  dtynan
 * Lots of changes for minor bug fixes, added functionality and the like.
 * Notably the following:
 *     o Added a 'db_findXXXfirst()' function for searching the entired
 * 	 table.
 *     o Added a 'db_runXXXquery()' function which will run an SQL
 * 	 statement (without parsing any arguments) and return the
 * 	 first match.
 *     o Fixed a bug where the include() m4 statement was being used
 * 	 more than once.
 *     o Put the 'EMIT' code at the bottom of the output file.
 *
 * Revision 1.3  2004/01/26 23:43:21  dtynan
 * Extensive changes to fix some M4 issues and some library issues.
 * Removed many of the functions which were used to parse data types
 * and made them inline instead.  Improved the M4 generator by adding
 * for loops.
 *
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
	struct	func	*fval;
};

%term	<nval>	VAL
%term	<sval>	STRING
%term	<ival>	IDENT

%term	PCENT ERROR
%term	KW_ARG KW_CODE KW_DELETE KW_DUMP KW_EMIT KW_FUNCTION KW_INSERT
%term	KW_FNAME KW_PROTO KW_QUERY KW_SEARCH KW_TABLE KW_TYPE KW_UPDATE

%term	KW_AUTOINCR KW_BIGINT KW_BLOB KW_CHAR KW_DECIMAL KW_DATE KW_DATETIME
%term	KW_DOUBLE KW_ENUM KW_FLOAT KW_INT KW_KEY KW_LONGBLOB KW_LONGTEXT
%term	KW_MEDBLOB KW_MEDINT KW_MEDTEXT KW_NATIONAL KW_NOT KW_NULL KW_NUMERIC
%term	KW_PREC KW_PRIMARY KW_REAL KW_SET KW_SMALLINT KW_TEXT KW_TIME KW_TSTAMP
%term	KW_TINYBLOB KW_TINYINT KW_TINYTEXT KW_UNIQUE KW_UNSIGNED KW_VARCHAR
%term	KW_YEAR

%term	',' '(' ')' '{' '}' '/'

%type	<nval>	number class opttype optquals optqual
%type	<sval>	string
%type	<ival>	ident
%type	<tval>	ntname otname
%type	<fval>	nfname

%%

dbow_file:	  /* Empty statement */
		| dbow_file dbow_stmnt
		;

dbow_stmnt:	  emit_stmnt
		| table_stmnt
		| function_stmnt
		| insert_stmnt
		| delete_stmnt
		| search_stmnt
		| update_stmnt
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
		{
			if (!$2)
				break;
			printf("Function is VALID!\n");
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

dump_stmnt:	  PCENT opttype KW_DUMP otname
		{
			if ($2)
				$4->flags |= FLAG_DUMP;
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

nfname:		  ident
		{
			$$ = curfunc = newfunction($1, 0);
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


func_defs:	  func_def
		| func_defs ',' func_def
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

func_def:	  KW_TABLE otname
		{
			printf("TABLE:[%s]\n", $2);
		}
		| KW_TYPE KW_INSERT
		{
			printf("TYPE-INSERT\n");
		}
		| KW_TYPE KW_DELETE
		{
			printf("TYPE-DELETE\n");
		}
		| KW_TYPE KW_SEARCH
		{
			printf("TYPE-SEARCH\n");
		}
		| KW_TYPE KW_UPDATE
		{
			printf("TYPE-UPDATE\n");
		}
		| KW_FNAME ident
		{
			printf("FNAME:[%s]\n", $2);
		}
		| KW_QUERY string
		{
			printf("QUERY:[%s]\n", $2);
		}
		| KW_ARG number ident
		{
			printf("ARG%d:[%s]\n", $2, $3);
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
			tlen = tprec = 0;
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
	{"arg",			KW_ARG},
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
	{"funcname",		KW_FNAME},
	{"not",			KW_NOT},
	{"null",		KW_NULL},
	{"numeric",		KW_NUMERIC},
	{"precision",		KW_PREC},
	{"primary",		KW_PRIMARY},
	{"proto",		KW_PROTO},
	{"query",		KW_QUERY},
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
	case '/':
		/*
		 * A special character - just return it as-is.
		 */
		return(*inptr++);

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
		fprintf(stderr, "\"%s\", line %d: ", filename, lineno);
	fprintf(stderr, "%s.\n", msg);
	nerrors++;
}
