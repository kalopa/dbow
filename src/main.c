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
 * Revision 1.5  2004/01/28 13:53:17  dtynan
 * Minor bug fixes and modifications prior to new release 0.3
 *
 * Revision 1.4  2004/01/26 23:43:21  dtynan
 * Extensive changes to fix some M4 issues and some library issues.
 * Removed many of the functions which were used to parse data types
 * and made them inline instead.  Improved the M4 generator by adding
 * for loops.
 *
 * Revision 1.3  2003/11/17 13:15:20  dtynan
 * Various changes to fix errors in the back-end code.
 *
 * Revision 1.2  2003/10/14 14:10:56  dtynan
 * Some fixes for SQL and C, as well as 'dnl' lines in the M4 templates to
 * reduce blank lines in the output.
 *
 * Revision 1.1  2003/10/14 13:00:28  dtynan
 * Major revision of the DBOW code to use M4 as a back-end instead of
 * hard-coding the output.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "dbowint.h"
#include "dbow.h"

void	usage();

char	*fofile;
char	*hofile;
char	*tofile;

extern	int	optind;
extern	int	opterr;
extern	char	*optarg;

/*
 * It all starts here...
 */
int
main(int argc, char *argv[])
{
	int i;
	char tmpfname[256];
	char *cp, *ifile;

	opterr = nflag = mflag = 0;
	active = NULL;
	fofile = hofile = NULL;
	/*
	 * Deal with the command-line options.
	 */
	while ((i = getopt(argc, argv, "h:t:o:xNm")) != -1) {
		switch (i) {
		case 'h':
			hofile = optarg;
			break;

		case 't':
			if ((active = findtype(optarg)) == NULL) {
				fprintf(stderr, "dbow: unsupported code type: %s\n", optarg); 
				exit(1);
			}
			break;

		case 'o':
			fofile = optarg;
			break;
#ifdef YYDEBUG
		case 'x':
			yydebug = 1;
			break;
#endif
		case 'm':
			mflag = 1;
			break;

		case 'N':
			nflag = 1;
			break;

		default:
			usage();
			break;
		}
	}
	/*
	 * The default generator is for C code.
	 */
	if (active == NULL && (active = findtype("c")) == NULL) {
		fprintf(stderr, "dbow: can't find default generator type.\n");
		exit(1);
	}
	/*
	 * Make sure we have an input file, and call the lexical
	 * analyzer to open it.  Report an error if that doesn't work.
	 */
	if ((argc - optind) != 1)
		usage();
	ifile = strdup(argv[optind]);
	if (lexopen(ifile) < 0) {
		fprintf(stderr, "dbow: ");
		perror(ifile);
		exit(1);
	}
	/*
	 * If no output file has been specified, then carve an
	 * output file by changing the <file>.d to <file>.<fext>
	 * where 'fext' is the generators file extension.
	 */
	if (fofile == NULL) {
		strncpy(tmpfname, ifile, sizeof(tmpfname) - 5);
		if ((cp = strrchr(tmpfname, '.')) != NULL &&
					cp[1] == 'd' && cp[2] == '\0') {
			*cp = '\0';
		} else
			cp = strchr(tmpfname, '\0');
		*cp++ = '.';
		if (mflag)
			strcpy(cp, "m4");
		else
			strcpy(cp, active->fext);
		if ((cp = strrchr(tmpfname, '/')) != NULL)
			cp++;
		else
			cp = tmpfname;
		fofile = strdup(cp);
	}
	/*
	 * Open the pipe to the M4 command.
	 * XXX - don't use M4 when generating SQL (for now!).
	 */
	if (mflag || active->cdtype == CDT_DBASE) {
		if (strcmp(fofile, "-") == 0)
			fofp = stdout;
		else if ((fofp = fopen(fofile, "w")) == NULL) {
			fprintf(stderr, "dbow: cannot open file for writing: ");
			perror(fofile);
			exit(1);
		}
		hofile = NULL;
	} else
		fofp = m4open(fofile, active);
	linesync(ifile, 1, fofp);
	if (hofile == NULL)
		hofp = fofp;
	else {
		hofp = m4open(hofile, active);
		linesync(ifile, 1, hofp);
	}
	tofp = NULL;
	/*
	 * Parse the input file using YACC and close the input stream.
	 * If there are no errors, call each table generator.
	 */
	nerrors = 0;
	yyparse();
	lexclose();
	if (nerrors == 0) {
		/*
		 * If we've diverted the prolog to a separate file, then
		 * we'll need to include it in the main file.
		 */
		if (fofp != hofp)
			fileinc(hofile, fofp);
		doproto(NULL, 0);
		docode(NULL, 0);
		/*
		 * Emit the epilog code.
		 */
		genepilog(fofp);
		if (hofp != fofp)
			genepilog(hofp);
		if (tofp != NULL) {
			fclose(tofp);
			if ((tofp = fopen(tofile, "r")) != NULL) {
				while ((i = fgetc(tofp)) != EOF)
					fputc(i, fofp);
				fclose(tofp);
			}
			unlink(tofile);
		}
	}
	/*
	 * Close out the output file (and delete it if it's bad),
	 * then exit.
	 */
	pclose(fofp);
	if (hofp != fofp)
		pclose(hofp);
	exit(nerrors > 0 ? 1 : 0);
}

/*
 *
 */
void
genfuncname(struct table *tp, char *cname, char *fname, int type)
{
	char tmp[512], *xp;
	struct column *cp = NULL;

	if (tp == NULL)
		return;
	if (cname != NULL && (cp = findcolumn(tp, cname)) == NULL) {
		yyerror("cannot find column within table");
		return;
	}
	switch (type) {
	case DBOW_INSERT:
		if (tp->ifname != NULL) {
			yyerror("multiple insert definitions for table");
			return;
		}
		xp = "insert";
		break;
	case DBOW_DELETE:
		if (cp == NULL)
			return;
		if (cp->dfname != NULL) {
			yyerror("multiple insert definitions for column");
			return;
		}
		xp = "delete";
		break;
	case DBOW_SEARCH:
		if (cp == NULL)
			return;
		if (cp->sfname != NULL) {
			yyerror("multiple insert definitions for column");
			return;
		}
		xp = "find";
		break;
	case DBOW_UPDATE:
		if (cp == NULL)
			return;
		if (cp->ufname != NULL) {
			yyerror("multiple insert definitions for column");
			return;
		}
		xp = "update";
		break;
	}
	if (fname == NULL) {
		if (type != DBOW_INSERT)
			sprintf(tmp, "db_%s%sby%s", xp, tp->name, cp->name);
		else
			sprintf(tmp, "db_%s%s", xp, tp->name);
		xp = tmp;
	} else
		xp = fname;
	switch (type) {
	case DBOW_INSERT:
		tp->ifname = strdup(xp);
		break;
	case DBOW_DELETE:
		cp->dfname = strdup(xp);
		break;
	case DBOW_SEARCH:
		cp->sfname = strdup(xp);
		break;
	case DBOW_UPDATE:
		cp->ufname = strdup(xp);
		break;
	}
}

/*
 * Generate prototype statements (C structures, etc).
 */
void
doproto(char *fname, int lineno)
{
	int sf, uf, df;
	static int protodone = 0;
	struct table *tp;
	struct column *cp, *sfc, *ufc, *dfc;

	/*
	 * Make sure we only execute once...
	 */
	if (protodone)
		return;
	protodone = 1;
	/*
	 * Do any table optimizations here...
	 */
	genprolog(fname, fofp);
	if (hofp != fofp)
		genprolog(fname, hofp);
	for (tp = getnexttable(NULL); tp != NULL; tp = getnexttable(tp)) {
		/*
		 * We always need an insert function.
		 */
		if (tp->ifname == NULL)
			genfuncname(tp, NULL, NULL, DBOW_INSERT);
		/*
		 * Make sure there is at least one search, delete and
		 * update function for each table.  -2 means we haven't
		 * seen anything yet.  -1 means there is a function
		 * definition already, and 0 or better is the column
		 * name for the first primary key.
		 */
		sf = uf = df = 0;
		sfc = ufc = dfc = NULL;
		for (cp = tp->chead; cp != NULL; cp = cp->next) {
			if (sf == 0) {
				if (cp->sfname != NULL) {
					sf = 1;
					sfc = NULL;
				} else if (sfc == NULL && cp->flags & FLAG_PRIKEY)
					sfc = cp;
			}
			if (uf == 0) {
				if (cp->ufname != NULL) {
					uf = 1;
					ufc = NULL;
				} else if (ufc == NULL && cp->flags & FLAG_PRIKEY)
					ufc = cp;
			}
			if (df == 0) {
				if (cp->dfname != NULL) {
					df = 1;
					ufc = NULL;
				} else if (dfc == NULL && cp->flags & FLAG_PRIKEY)
					dfc = cp;
			}
		}
		/*
		 * Build each default function (where needed).
		 */
		if (sfc != NULL)
			genfuncname(tp, sfc->name, NULL, DBOW_SEARCH);
		if (ufc != NULL)
			genfuncname(tp, ufc->name, NULL, DBOW_UPDATE);
		if (dfc != NULL)
			genfuncname(tp, dfc->name, NULL, DBOW_DELETE);
		/*
		 * Now emit the actual M4 definitions for each table...
		 */
		gendefs(tp, fofile, fofp);
		if (fofp != hofp)
			gendefs(tp, hofile, hofp);
	}
	/*
	 * Generate the middle section, including the "m4 include"...
	 */
	genmidsect(fname, fofp);
	if (hofp != fofp)
		genmidsect(fname, hofp);
	/*
	 * Now call the code-specific prototype generator for each table.
	 */
	if (lineno > 0)
		linesync(fname, lineno, hofp);
	tp = getnexttable(NULL);
	while (tp != NULL) {
		genstr(tp, hofp);
		tp = getnexttable(tp);
	}
}

/*
 * Generate code statements.
 */
void
docode(char *fname, int lineno)
{
	static int codedone = 0;
	struct table *tp;

	/*
	 * Make sure we only execute once...
	 */
	if (codedone)
		return;
	codedone = 1;
	/*
	 * Call the code-specific generator for each table.
	 */
	if (lineno > 0)
		linesync(fname, lineno, fofp);
	tp = getnexttable(NULL);
	while (tp != NULL) {
		gencode(tp, fofp);
		tp = getnexttable(tp);
	}
}

/*
 *
 */
int
gentmpf()
{
	if ((tofile = strdup("/tmp/dbowXXXXXX")) == NULL)
		return(-1);
	if (mkstemp(tofile) < 0)
		return(-1);
	return((tofp = fopen(tofile, "w")) == NULL ? -1 : 0);
}

/*
 *
 */
void
usage()
{
	fprintf(stderr, "Usage: dbow [-Nhm][-t type][-o file] file\n");
	exit(2);
}
