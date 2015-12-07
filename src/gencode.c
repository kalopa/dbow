/*
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dbowint.h"

#define M4	"/usr/bin/m4"

#define BTYPE_FILL	0
#define BTYPE_ALLOC	1
#define BTYPE_FREE	2
#define BTYPE_INSERT	3
#define NBASETYPES	4

char	*basetype[NBASETYPES] = {
	"FILL", "ALLOC", "FREE", "INSERT"
};

char	*codetype[NTYPES] = {
	"TYPE_TINYINT", "TYPE_SMALLINT", "TYPE_MEDINT", "TYPE_INT",
	"TYPE_BIGINT", "TYPE_FLOAT", "TYPE_DOUBLE", "TYPE_NUMERIC", "TYPE_DATE",
	"TYPE_TIME", "TYPE_DATETIME", "TYPE_TSTAMP", "TYPE_YEAR", "TYPE_CHAR",
	"TYPE_VARCHAR", "TYPE_TINYBLOB", "TYPE_TINYTEXT", "TYPE_BLOB",
	"TYPE_TEXT", "TYPE_MEDBLOB", "TYPE_MEDTEXT", "TYPE_LONGBLOB",
	"TYPE_LONGTEXT", "TYPE_ENUM", "TYPE_SET"
};

/*
 *
 */
static void
setfield(char *label, char *val, FILE *fp)
{
	fprintf(fp, "%s(%s)\n", label, val);
}

/*
 *
 */
static void
dofunction(char *label, struct table *tp, char *fname, int fno, FILE *fp)
{
	fprintf(fp, "%s(%s,`%s'", label, tp->name, fname);
	if (fno >= 0)
		fprintf(fp, ",%d", fno);
	fprintf(fp, ")\n");
}

/*
 *
 */
static void
setbooltype(char *tname, char *fname, int num, FILE *fp)
{
	fprintf(fp, "define(STR_%s_%s%d)\n", tname, fname, num);
}

/*
 *
 */
static void
setinttype(char *tname, char *fname, int num, int arg, FILE *fp)
{
	fprintf(fp, "define(STR_%s_%s%d,%d)\n", tname, fname, num, arg);
}

/*
 *
 */
static void
setstrtype(char *tname, char *fname, int num, char *arg, FILE *fp)
{
	fprintf(fp, "define(STR_%s_%s%d,%s)\n", tname, fname, num, arg);
}

/*
 *
 */
static void
divert(int flag, FILE *fp)
{
	fprintf(fp, "divert(%d)dnl\n", flag ? -1 : 0);
}

/*
 *
 */
void
genprolog(char *fname, FILE *fp)
{
	/*
	 * XXX - don't use M4 when generating SQL (for now!).
	 */
	if (active->cdtype == CDT_DBASE)
		return;
	divert(1, fp);
	fprintf(fp, "dnl\ndnl Intermediate M4 file produced by DBOW ");
	fprintf(fp, "from \"%s\"\ndnl Do not edit this file.", fname);
	fprintf(fp, "  Combine it with the appropriate DBOW M4 file\n");
	fprintf(fp, "dnl to produce the appropriate output.\ndnl\n");
}

/*
 *
 */
void
gendefs(struct table *tp, char *fname, FILE *fp)
{
	int fno;
	struct column *cp;

	/*
	 * XXX - don't use M4 when generating SQL (for now!).
	 */
	if (active->cdtype == CDT_DBASE)
		return;
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		setstrtype(tp->name, "NAME", fno = cp->fno, cp->name, fp);
		setstrtype(tp->name, "TYPE", fno, codetype[cp->type], fp);
		setinttype(tp->name, "LEN", fno, cp->length, fp);
		if (cp->flags & FLAG_AUTOINC)
			setbooltype(tp->name, "AUTO", fno, fp);
		if (cp->flags & FLAG_UNSIGNED)
			setbooltype(tp->name, "UNS", fno, fp);
		if (cp->flags & FLAG_PRIKEY)
			setbooltype(tp->name, "PKEY", fno, fp);
		if (cp->flags & FLAG_NOTNULL)
			setbooltype(tp->name, "NULL", fno, fp);
		if (cp->flags & FLAG_UNIQUE)
			setbooltype(tp->name, "UNQ", fno, fp);
	}
	setinttype(tp->name, "NELEM", 0, fno, fp);
}

/*
 *
 */
void
genmidsect(char *fname, FILE *fp)
{
	/*
	 * XXX - don't use M4 when generating SQL (for now!).
	 */
	if (active->cdtype == CDT_DBASE)
		return;
	divert(0, fp);
	m4include(fp);
	setfield("FILE_PROLOG", fname, fp);
}

/*
 *
 */
void
genstr(struct table *tp, FILE *fp)
{
	char *dname;
	struct column *cp;

	/*
	 * XXX - don't use M4 when generating SQL (for now!).
	 */
	if (active->cdtype == CDT_DBASE) {
		generatesql(tp, fp);
		return;
	}
	setfield("STRUCT_BODY", tp->name, fp);
	setfield("INIT_PROTO", tp->name, fp);
	if (tp->ifname != NULL)
		dofunction("INSERT_PROTO", tp, tp->ifname, -1, fp);
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->dfname != NULL)
			dofunction("DELETE_PROTO", tp, cp->dfname, cp->fno, fp);
		if (cp->sfname != NULL)
			dofunction("SEARCH_PROTO", tp, cp->sfname, cp->fno, fp);
		if (cp->ufname != NULL)
			dofunction("UPDATE_PROTO", tp, cp->ufname, cp->fno, fp);
	}
	if (tp->flags & FLAG_DUMP) {
		if ((dname = malloc(strlen(tp->name) + 8)) == NULL)
			return;
		sprintf(dname, "db_dump%s", tp->name);
		dofunction("DUMP_PROTO", tp, dname, -1, fp);
		free(dname);
	}
}

/*
 *
 */
void
gencode(struct table *tp, FILE *fp)
{
	char *dname;
	struct column *cp;

	/*
	 * XXX - don't use M4 when generating SQL (for now!).
	 */
	if (active->cdtype == CDT_DBASE)
		return;
	setfield("INIT_BODY", tp->name, fp);
	if (tp->ifname != NULL)
		dofunction("INSERT_BODY", tp, tp->ifname, -1, fp);
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->dfname != NULL)
			dofunction("DELETE_BODY", tp, cp->dfname, cp->fno, fp);
		if (cp->sfname != NULL)
			dofunction("SEARCH_BODY", tp, cp->sfname, cp->fno, fp);
		if (cp->ufname != NULL)
			dofunction("UPDATE_BODY", tp, cp->ufname, cp->fno, fp);
	}
	if (tp->flags & FLAG_DUMP) {
		if ((dname = malloc(strlen(tp->name) + 8)) == NULL)
			return;
		sprintf(dname, "db_dump%s", tp->name);
		dofunction("DUMP_BODY", tp, dname, -1, fp);
		free(dname);
	}
}

/*
 *
 */
void
genepilog(FILE *fp)
{
	/*
	 * XXX - don't use M4 when generating SQL (for now!).
	 */
	if (active->cdtype == CDT_DBASE)
		return;
	fprintf(fp, "FILE_EPILOG\n");
}
