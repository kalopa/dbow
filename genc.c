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
 * Revision 1.2  2003/07/28 21:48:39  dtynan
 * Minor tweaks, including fixing some gensync issues.
 *
 * Revision 1.1  2003/07/28 21:31:57  dtynan
 * First pass at an intelligent front-end for databases.
 */

#include <stdio.h>

#include "dbowint.h"

char	*ctypes[NTYPES] = {
	"char", "short", "int", "int", "long", "float", "double",
	"char *", "int", "int", "int", "int", "int", "char",
	"char *", "char *", "char *", "char *", "char *", "char *",
	"char *", "char *", "char *", "<error>", "<error>"
};

char	*ftypes[NTYPES] = {
	"int", "int", "int", "int", "long", "float", "double", "dchar", "date",
	"date", "date", "date", "int", "char", "char", "char", "char", "char",
	"char", "char", "char", "char", "char"
};

char	stypes[NTYPES+1] = "dddddffsdddddssssssssss";

char *
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
 *
 */
void
gencsearch(struct table *tp, struct column *cp, FILE *fp)
{
	fprintf(fp, "\nstruct %s *\n%sfind%sby%s(", tp->pfx, prefix,
						tp->name, cp->name);
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
 *
 */
int
line_c(char *fname, int lno, FILE *fp)
{
	fprintf(fp, "#line %d \"%s\"\n", lno, fname);
}

/*
 *
 */
int
str_c(struct table *tp, FILE *fp)
{
	struct column *cp;

	fprintf(fp, "/*\n * Structure definition for ");
	fprintf(fp, "SQL table %s\n */\n", tp->name);
	fprintf(fp, "\nstruct %s {\n", tp->pfx);
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		fprintf(fp, "\t%s\t%s", cdef(cp), cp->name);
		if (cp->type == TYPE_CHAR)
			fprintf(fp, "[%d]", cp->length);
		fprintf(fp, ";\n");
	}
	fprintf(fp, "};\n\n/*\n * Prototypes\n */\n");
	fprintf(fp, "struct %s\t*%salloc();\n", tp->pfx, tp->pfx);
	fprintf(fp, "void\t\t%sfree(struct %s *);\n", tp->pfx, tp->pfx);
	fprintf(fp, "int\t\t%sinsert(dbow_conn *, struct %s *);\n", tp->pfx, tp->pfx);
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if ((cp->flags & FLAG_SEARCH) == 0)
			continue;
		fprintf(fp, "struct %s\t*%sfind%sby%s(", tp->pfx, prefix,
							tp->name, cp->name);
		fprintf(fp, "dbow_conn *, %s);\n", cdef(cp));
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

	fprintf(fp, "struct %s *\n%salloc()\n{\n", tp->pfx, tp->pfx);
	fprintf(fp, "\tstruct %s *p;\n\n", tp->pfx);
	fprintf(fp, "\tif ((p = (struct %s *)malloc(sizeof(struct ", tp->pfx);
	fprintf(fp, "%s))) == NULL)\n\t\treturn(NULL);\n", tp->pfx);
	fprintf(fp, "\tmemset((char *)p, 0, sizeof(struct ");
	fprintf(fp, "%s));\n\treturn(p);\n}\n\n", tp->pfx);

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
			fprintf(fp, "\t\tfree(p->%s);\n", cp->name);
			break;
		}
	}
	fprintf(fp, "\tfree((char *)p);\n}\n\n");

	fprintf(fp, "int\n%sinsert(dbow_conn *c, struct %s *p)\n{\n", tp->pfx, tp->pfx);
	fprintf(fp, "\tint i;\n\tchar q[1024];\n\tMYSQL_RES *rsp;\n\n");
	fprintf(fp, "\tstrcpy(q, \"insert into %s ", tp->name);
	fprintf(fp, " values(\");\n\tif ");
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
	fprintf(fp, "\treturn(0);\n}\n\n");

	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->flags & FLAG_SEARCH)
			gencsearch(tp, cp, fp);
	}
}
