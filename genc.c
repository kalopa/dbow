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
 * Revision 1.7  2003/07/29 15:22:49  dtynan
 * Revised copyright prior to first public release.
 *
 * Revision 1.6  2003/07/29 15:17:29  dtynan
 * Lots and lots of changes.
 *
 * Revision 1.5  2003/07/29 00:36:04  dtynan
 * Formatting errors.
 *
 * Revision 1.4  2003/07/29 00:30:03  dtynan
 * Lots of changes.
 *
 * Revision 1.3  2003/07/28 23:44:59  dtynan
 * Fixed a bug with the separate include file not being invoked for a split
 * generation.  Also made sure that dbow.h only got invoked once.  Fixed a
 * problem with type 'char[]' which can't be assigned.
 *
 * Revision 1.2  2003/07/28 21:48:39  dtynan
 * Minor tweaks, including fixing some gensync issues.
 *
 * Revision 1.1  2003/07/28 21:31:57  dtynan
 * First pass at an intelligent front-end for databases.
 */

#include <stdio.h>

#include "dbowint.h"

/*
 * C types for structs and data elements.
 */
static	char	*ctypes[NTYPES] = {
	"char", "short", "int", "int", "long", "float", "double",
	"char *", "int", "int", "int", "int", "int", "char",
	"char *", "char *", "char *", "char *", "char *", "char *",
	"char *", "char *", "char *", "<error>", "<error>"
};

/*
 * C function names within the dbow library.
 */
static	char	*ftypes[NTYPES] = {
	"int", "int", "int", "int", "long", "float", "double", "dchar", "date",
	"date", "date", "date", "int", "char", "char", "char", "char", "char",
	"char", "char", "char", "char", "char"
};

/*
 * Format types for printf()
 */
static	char	stypes[NTYPES+1] = "dddddffsdddddssssssssss";

/*
 * Create a C definition statement based on the column type.
 */
static char *
cdef(struct column *cp)
{
	static char tmp[512];

	tmp[0] = '\0';
	if (cp->flags & FLAG_UNSIGNED)
		strcpy(tmp, "unsigned ");
	strcat(tmp, ctypes[cp->type]);
	return(tmp);
}

/*
 * Generate a function for deleting a row from the database based on
 * the specified argument name.
 */
static void
gencdelete(struct table *tp, struct column *cp, FILE *fp)
{
	fprintf(fp, "\nint\n%s(", cp->dfname);
	fprintf(fp, "dbow_conn *c, %s %s)\n{\n", cdef(cp), cp->name);
	fprintf(fp, "\tchar q[100];\n");
	fprintf(fp, "\tsprintf(q,\"DELETE FROM %s WHERE ", tp->name);
	fprintf(fp, "%s='%%%c'\",%s);\n", cp->name, stypes[cp->type], cp->name);
	fprintf(fp, "\tif (dbow_query(c, q) != 0)\n\t\treturn(-1);\n");
	fprintf(fp, "\treturn(0);\n}\n");
}

/*
 * Generate a function for searching the database based on the specified
 * column name.
 */
static void
gencsearch(struct table *tp, struct column *cp, FILE *fp)
{
	fprintf(fp, "\nstruct %s *\n%s(", tp->pfx, cp->sfname);
	fprintf(fp, "dbow_conn *c, %s %s)\n{\n", cdef(cp), cp->name);
	fprintf(fp, "\tstruct %s *p = NULL;\n", tp->pfx);
	fprintf(fp, "\tchar q[100];\n");
	fprintf(fp, "\tdbow_res *rsp;\n");
	fprintf(fp, "\tdbow_row row;\n\n");
	fprintf(fp, "\tsprintf(q,\"SELECT * FROM %s WHERE ", tp->name);
	fprintf(fp, "%s='%%%c'\",%s);\n", cp->name, stypes[cp->type], cp->name);
	fprintf(fp, "\tif (dbow_query(c, q) != 0)\n\t\treturn(NULL);\n");
	fprintf(fp, "\tif ((rsp = dbow_store_result(c)) == NULL)\n");
	fprintf(fp, "\t\treturn(NULL);\n");
	fprintf(fp, "\tif ((row = dbow_fetch_row(rsp)) != NULL) {\n");
	fprintf(fp, "\t\tif ((p = %salloc()) != NULL)\n", tp->pfx);
	fprintf(fp, "\t\t\t_dbfill_%s(row, p);\n", tp->name);
	fprintf(fp, "\t}\n\tdbow_free_result(rsp);\n");
	fprintf(fp, "\treturn(p);\n}\n");
}

/*
 * Generate a function for updating a row in the database based on
 * the specified argument name.
 */
static void
gencupdate(struct table *tp, struct column *cp, FILE *fp)
{
	int next;
	struct column *xcp;

	fprintf(fp, "\nint\n%s(", cp->ufname);
	fprintf(fp, "dbow_conn *c, struct %s *p)\n{\n", tp->pfx);
	fprintf(fp, "\tchar q[1024];\n\n");
	next = 0;
	for (xcp = tp->chead; xcp != NULL; xcp = xcp->next) {
		if (!next) {
			fprintf(fp, "\tsprintf(q,\"UPDATE %s SET %s='\");\n",
						tp->name, xcp->name);
			next = 1;
		} else {
			fprintf(fp, "\tdbow_strcat(q,\"', %s='\",sizeof(q));\n",
						xcp->name);
		}
		fprintf(fp, "\tdbow_i%s(q,p->%s,sizeof(q));\n",
					ftypes[xcp->type], xcp->name);
	}
	fprintf(fp, "\tstrcat(q,\"' WHERE %s='%%%c'\",p->%s);\n", cp->name,
						stypes[cp->type], cp->name);
	fprintf(fp, "\tif (dbow_query(c, q) != 0)\n\t\treturn(-1);\n");
	fprintf(fp, "\treturn(0);\n}\n");
}

/*
 * First of the three exposed functions.  This one emits a synchronization
 * line to the output file in a code-dependent way.
 */
int
line_c(char *fname, int lno, FILE *fp)
{
	fprintf(fp, "#line %d \"%s\"\n", lno, fname);
}

/*
 * Function to generate all the prototype/structure code.  Think of
 * this as the function which directs output to the C include file.
 */
int
str_c(struct table *tp, FILE *fp)
{
	struct column *cp;

	/*
	 * Start with the structure definition for the table.
	 */
	fprintf(fp, "\n/*\n * Structure definition for ");
	fprintf(fp, "SQL table %s\n */\n", tp->name);
	fprintf(fp, "struct %s {\n", tp->pfx);
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		fprintf(fp, "\t%s\t%s", cdef(cp), cp->name);
		if (cp->type == TYPE_CHAR)
			fprintf(fp, "[%d]", cp->length);
		fprintf(fp, ";\n");
	}
	/*
	 * Now do the exported function prototypes starting with the
	 * well-defined ones you get for free.
	 */
	fprintf(fp, "};\n\n/*\n * Prototypes\n */\n");
	fprintf(fp, "struct %s *%salloc();\n", tp->pfx, tp->pfx);
	fprintf(fp, "void %sfree(struct %s *);\n", tp->pfx, tp->pfx);
	fprintf(fp, "int %s(dbow_conn *, struct %s *);\n", tp->ifname, tp->pfx);
	/*
	 * Do the delete functions...
	 */
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->dfname == NULL)
			continue;
		fprintf(fp, "int %s(", cp->dfname);
		fprintf(fp, "dbow_conn *, %s);\n", cdef(cp));
	}
	/*
	 * Do the search functions...
	 */
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->sfname == NULL)
			continue;
		fprintf(fp, "struct %s *%s(", tp->pfx, cp->sfname);
		fprintf(fp, "dbow_conn *, %s);\n", cdef(cp));
	}
	/*
	 * Do the update functions...
	 */
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->ufname == NULL)
			continue;
		fprintf(fp, "int %s(", cp->ufname);
		fprintf(fp, "dbow_conn *, struct %s *p);\n", tp->pfx);
	}
	/*
	 * Do we need a dump function?
	 */
	if (tp->flags & FLAG_DUMP) {
		fprintf(fp, "void db_dump%s(struct %s *", tp->name, tp->pfx);
		fprintf(fp, ", FILE *);\n");
	}
}

/*
 *
 */
int
code_c(struct table *tp, FILE *fp)
{
	int i;
	struct column *cp;

	/*
	 * Do the hard-coded functions, like the one that fills the
	 * struct from the SQL data
	 */
	fprintf(fp, "\nstatic void\n_dbfill_%s(dbow_row row, ", tp->name);
	fprintf(fp, "struct %s *p)\n{\n", tp->pfx);
	for (i = 0, cp = tp->chead; cp != NULL; i++, cp = cp->next) {
		if (cp->type == TYPE_CHAR) {
			fprintf(fp, "\tdb_fchrs(p->%s, row, %d);\n",
					cp->name, i);
		} else {
			fprintf(fp, "\tp->%s = dbow_f%s(row, %d);\n",
					cp->name, ftypes[cp->type], i);
		}
	}
	fprintf(fp, "}\n\n");
	/*
	 * We always define an alloc() function.
	 */
	fprintf(fp, "struct %s *\n%salloc()\n{\n", tp->pfx, tp->pfx);
	fprintf(fp, "\tstruct %s *p;\n\n", tp->pfx);
	fprintf(fp, "\tif ((p = (struct %s *)dbow_alloc(sizeof(struct ", tp->pfx);
	fprintf(fp, "%s))) == NULL)\n\t\treturn(NULL);\n", tp->pfx);
	fprintf(fp, "\tmemset((char *)p, 0, sizeof(struct ");
	fprintf(fp, "%s));\n\treturn(p);\n}\n\n", tp->pfx);
	/*
	 * We always define a free() function.
	 */
	fprintf(fp, "void\n%sfree(struct %s *p)\n{\n", tp->pfx, tp->pfx);
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		switch (cp->type) {
		case TYPE_NUMERIC:
		case TYPE_CHAR:
		case TYPE_VARCHAR:
		case TYPE_TINYBLOB:
		case TYPE_TINYTEXT:
		case TYPE_BLOB:
		case TYPE_TEXT:
		case TYPE_MEDBLOB:
		case TYPE_MEDTEXT:
		case TYPE_LONGBLOB:
		case TYPE_LONGTEXT:
			fprintf(fp, "\tif (p->%s != NULL)\n", cp->name);
			fprintf(fp, "\t\tdbow_free(p->%s);\n", cp->name);
			break;
		}
	}
	fprintf(fp, "\tdbow_free((char *)p);\n}\n\n");
	/*
	 * Likewise, there's always an insert function.
	 */
	fprintf(fp, "int\n%s(dbow_conn *c, struct %s *p)\n{\n", tp->ifname, tp->pfx);
	fprintf(fp, "\tint i;\n\tchar q[1024];\n\tMYSQL_RES *rsp;\n\n");
	fprintf(fp, "\tstrcpy(q, \"INSERT INTO %s", tp->name);
	fprintf(fp, " VALUES(\");\n\tif ");
	for (i = '(', cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->next != NULL) {
			fprintf(fp, "%c\tdbow_i%s(q,p->%s,sizeof(q)) < 0 ||\n",
						i, ftypes[cp->type], cp->name);
		} else {
			fprintf(fp, "%c\t(i=dbow_i%s(q,p->%s,sizeof(q)))<0)\n",
						i, ftypes[cp->type], cp->name);
		}
		i = '\t';
	}
	fprintf(fp, "\t\t\treturn(-1);\n");
	fprintf(fp, "\tq[i-1] = ')';\n\tif (dbow_query(c,q) != 0)\n");
	fprintf(fp, "\t\treturn(-1);\n");
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->flags & FLAG_AUTOINC) {
			fprintf(fp, "\tif (p->%s == 0)\n\t\tp->", cp->name);
			fprintf(fp, "%s = dbow_insertid(c);\n", cp->name);
			break;
		}
	}
	fprintf(fp, "\treturn(0);\n}\n");
	/*
	 * Do delete functions...
	 */
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->dfname != NULL)
			gencdelete(tp, cp, fp);
	}
	/*
	 * Do search functions...
	 */
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->sfname != NULL)
			gencsearch(tp, cp, fp);
	}
	/*
	 * Do update functions...
	 */
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->ufname != NULL)
			gencupdate(tp, cp, fp);
	}
	/*
	 * Create the dump function (if needed)
	 */
	if (tp->flags & FLAG_DUMP) {
		fprintf(fp, "\nvoid\ndb_dump%s(struct %s *p",tp->name,tp->pfx);
		fprintf(fp, ", FILE *fp)\n{\n\tfprintf(fp, \"Dump of record ");
		fprintf(fp, "from table %s:-\\n\");\n", tp->name);
		for (cp = tp->chead; cp != NULL; cp = cp->next) {
			switch (cp->type) {
			case TYPE_NUMERIC:
			case TYPE_CHAR:
			case TYPE_VARCHAR:
			case TYPE_TINYBLOB:
			case TYPE_TINYTEXT:
			case TYPE_BLOB:
			case TYPE_TEXT:
			case TYPE_MEDBLOB:
			case TYPE_MEDTEXT:
			case TYPE_LONGBLOB:
			case TYPE_LONGTEXT:
				fprintf(fp, "\tif (p->%s != NULL)\n", cp->name);
				fprintf(fp, "\t\tfprintf(fp, \"%s", cp->name);
				fprintf(fp, " = \\\"%%s\\\"\\n\", p->");
				fprintf(fp, "%s);\n", cp->name);
				break;
			case TYPE_TINYINT:
			case TYPE_SMALLINT:
			case TYPE_MEDINT:
			case TYPE_INT:
			case TYPE_BIGINT:
			case TYPE_YEAR:
				fprintf(fp, "\tfprintf(fp, \"%s", cp->name);
				fprintf(fp, " = %%d\\n\", p->%s);\n", cp->name);
				break;

			case TYPE_FLOAT:
			case TYPE_DOUBLE:
				fprintf(fp, "\tfprintf(fp, \"%s", cp->name);
				fprintf(fp, " = %%f\\n\", p->%s);\n", cp->name);
				break;

			case TYPE_DATE:
			case TYPE_TIME:
			case TYPE_DATETIME:
			case TYPE_TSTAMP:
				fprintf(fp, "\tfprintf(fp, \"%s", cp->name);
				fprintf(fp, " = %%d\\n\", p->%s);\n", cp->name);
				break;

			}
		}
		fprintf(fp, "}\n");
	}
}
