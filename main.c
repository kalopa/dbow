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
 * Revision 1.3  2003/07/28 23:44:59  dtynan
 * Fixed a bug with the separate include file not being invoked for a split
 * generation.  Also made sure that dbow.h only got invoked once.  Fixed a
 * problem with type 'char[]' which can't be assigned.
 *
 * Revision 1.2  2003/07/28 21:48:40  dtynan
 * Minor tweaks, including fixing some gensync issues.
 *
 * Revision 1.1  2003/07/28 21:31:58  dtynan
 * First pass at an intelligent front-end for databases.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "dbowint.h"
#include "dbow.h"

void	usage();

char	tmpfname[256];
char	*fofile;
char	*hofile;

extern	int	optind;
extern	int	opterr;
extern	char	*optarg;

/*
 * It all starts here...
 */
int
main(int argc, char *argv[])
{
	int i, nflag = 0;
	char *cp, *ifile;
	FILE *fp;

	prefix = "db_";
	opterr = 0;
	active = NULL;
	fofile = hofile = NULL;
	/*
	 * Deal with the command-line options.
	 */
	while ((i = getopt(argc, argv, "h:t:p:o:xN")) != -1) {
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

		case 'p':
			prefix = optarg;
			break;

		case 'o':
			fofile = optarg;
			break;
#ifdef YYDEBUG
		case 'x':
			yydebug = 1;
			break;
#endif
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
	if (nflag)
		active->gensync = NULL;
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
		strcpy(cp, active->fext);
		fofile = strdup(tmpfname);
	}
	/*
	 * If the output file is '-', then emit the generated code to
	 * stdout, otherwise open the output file for writing.
	 */
	if (strcmp(fofile, "-") == 0)
		fofp = stdout;
	else if ((fofp = fopen(fofile, "w")) == NULL) {
		fprintf(stderr, "dbow: ");
		perror(fofile);
		exit(1);
	}
	if (active->gensync != NULL)
		active->gensync(ifile, 1, fofp);
	if (hofile == NULL)
		hofp = fofp;
	else {
		if ((hofp = fopen(hofile, "w")) == NULL) {
			fprintf(stderr, "dbow: ");
			perror(hofile);
			exit(1);
		}
		if (active->gensync != NULL)
			active->gensync(ifile, 1, hofp);
	}
	/*
	 * Emit the prolog code.
	 */
	if (active->prolog != NULL) {
		sprintf(tmpfname, "%s/%s", DBOW, active->prolog);
		if ((fp = fopen(tmpfname, "r")) != NULL) {
			if (active->gensync != NULL) {
				active->gensync(tmpfname, 1, fofp);
				if (hofp != fofp)
					active->gensync(tmpfname, 1, hofp);
			}
			while ((i = fgetc(fp)) != EOF) {
				fputc(i, fofp);
				if (hofp != fofp)
					fputc(i, hofp);
			}
			fclose(fp);
		}
	}
	/*
	 * If we've diverted the prolog to a separate file, then
	 * we'll need to include it in the main file.
	 */
	if (hofile != NULL)
		fprintf(fofp, "#include \"%s\"\n", hofile);
	/*
	 * Parse the input file using YACC and close the input stream.
	 * If there are no errors, call each table generator.
	 */
	nerrors = 0;
	yyparse();
	lexclose();
	if (nerrors == 0) {
		doproto(NULL, 0);
		docode(NULL, 0);
		/*
		 * Emit the epilog code.
		 */
		if (active->epilog != NULL) {
			sprintf(tmpfname, "%s/%s", DBOW, active->epilog);
			if ((fp = fopen(tmpfname, "r")) != NULL) {
				if (active->gensync != NULL) {
					active->gensync(tmpfname, 1, fofp);
					if (hofp != fofp)
						active->gensync(tmpfname, 1, hofp);
				}
				while ((i = fgetc(fp)) != EOF) {
					fputc(i, fofp);
					if (hofp != fofp)
						fputc(i, hofp);
				}
				fclose(fp);
			}
		}
	}
	/*
	 * Close out the output file (and delete it if it's bad),
	 * then exit.
	 */
	if (fofp != stdout) {
		fclose(fofp);
		if (nerrors)
			unlink(fofile);
	}
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
	case ACTION_INSERT:
		if (tp->ifname != NULL) {
			yyerror("multiple insert definitions for table");
			return;
		}
		xp = "insert";
		break;
	case ACTION_DELETE:
		if (cp == NULL)
			return;
		if (cp->dfname != NULL) {
			yyerror("multiple insert definitions for table");
			return;
		}
		xp = "delete";
		break;
	case ACTION_SEARCH:
		if (cp == NULL)
			return;
		if (cp->sfname != NULL) {
			yyerror("multiple insert definitions for table");
			return;
		}
		xp = "find";
		break;
	case ACTION_UPDATE:
		if (cp == NULL)
			return;
		if (cp->ufname != NULL) {
			yyerror("multiple insert definitions for table");
			return;
		}
		xp = "update";
		break;
	}
	if (fname == NULL) {
		if (type != ACTION_INSERT)
			sprintf(tmp, "%s%s%sby%s",prefix,xp,tp->name,cp->name);
		else
			sprintf(tmp, "%s%s%s",prefix,xp,tp->name);
		xp = tmp;
	} else
		xp = fname;
	switch (type) {
	case ACTION_INSERT:
		tp->ifname = strdup(xp);
		break;
	case ACTION_DELETE:
		cp->dfname = strdup(xp);
		break;
	case ACTION_SEARCH:
		cp->sfname = strdup(xp);
		break;
	case ACTION_UPDATE:
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
	static int protodone = 0;
	struct table *tp;

	/*
	 * Make sure we only execute once...
	 */
	if (protodone)
		return;
	protodone = 1;
	/*
	 * Do any table optimizations here...
	 */
	for (tp = getnexttable(NULL); tp != NULL; tp = getnexttable(tp)) {
		/*
		 * We always need an insert function.
		 */
		if (tp->ifname == NULL)
			genfuncname(tp, NULL, NULL, ACTION_INSERT);
	}
	/*
	 * Now call the code-specific prototype generator for each table.
	 */
	if (active->gensync != NULL && lineno > 0)
		active->gensync(fname, lineno, hofp);
	if (active->genstr != NULL) {
		tp = getnexttable(NULL);
		while (tp != NULL) {
			active->genstr(tp, hofp);
			tp = getnexttable(tp);
		}
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
	if (active->gensync != NULL && lineno > 0)
		active->gensync(fname, lineno, fofp);
	if (active->gencode != NULL) {
		tp = getnexttable(NULL);
		while (tp != NULL) {
			active->gencode(tp, fofp);
			tp = getnexttable(tp);
		}
	}
}

/*
 *
 */
void
usage()
{
	fprintf(stderr, "Usage: dbow [-t type][-p prefix][-h file][-o file][-N] file\n");
	exit(2);
}
