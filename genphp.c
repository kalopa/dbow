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
 * Revision 1.3  2003/07/28 23:14:36  dtynan
 * Fleshed out the PHP code generator.
 *
 * Revision 1.2  2003/07/28 21:48:40  dtynan
 * Minor tweaks, including fixing some gensync issues.
 *
 * Revision 1.1  2003/07/28 21:31:57  dtynan
 * First pass at an intelligent front-end for databases.
 */

#include <stdio.h>

#include "dbowint.h"

void
genphpsearch(struct table *tp, struct column *cp, FILE *fp)
{
	fprintf(fp, "\nfunction %s($conn, $%s)\n{\n", cp->sfname, cp->name);
	fprintf(fp, "\tglobal $FALSE;\n\n");
	fprintf(fp, "\tif ($r = $conn->doquery(\"SELECT * FROM ");
	fprintf(fp, "%s WHERE %s='$%s'\")) {\n", tp->name, cp->name, cp->name);
	fprintf(fp, "\t\t$this->setfields($r);\n\t}\n");
	fprintf(fp, "\treturn $r;\n}\n");
}

/*
 *
 */
int
code_php(struct table *tp, FILE *fp)
{
	struct column *cp;

	fprintf(fp, "class %s {\n", tp->pfx);
	for (cp = tp->chead; cp != NULL; cp = cp->next)
		fprintf(fp, "\tvar $%s;\n", cp->name);
	fputc('\n', fp);

	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		fprintf(fp, "function get_%s() { return ", cp->name);
		fprintf(fp, "$this->%s; }\n", cp->name);
		fprintf(fp, "function set_%s($val) { ", cp->name);
		fprintf(fp, "$this->%s = $val; }\n", cp->name);
	}

	fprintf(fp, "\nfunction setfields($r)\n{\n");
	fprintf(fp, "\t$s = mysql_fetch_array($r);\n");
	for (cp = tp->chead; cp != NULL; cp = cp->next)
		fprintf(fp, "\t$this->%s = $s[\"%s\"];\n", cp->name, cp->name);
	fprintf(fp, "}\n");

	fprintf(fp, "\nfunction insert()\n{\n");
	fprintf(fp, "\tglobal $FALSE;\n\n");
	fprintf(fp, "\tif ($r = $conn->doquery(\"INSERT INTO %s ", tp->name);
	fprintf(fp, "VALUES(");
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		fprintf(fp, "'$this->%s'", cp->name);
		if (cp->next != NULL)
			fputc(',', fp);
	}
	fprintf(fp, ")\")) {\n");
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->flags & FLAG_AUTOINC) {
			fprintf(fp, "\t\tif ($this->%s == 0)\n", cp->name);
			fprintf(fp, "\t\t\t$this->%s = mysql_insert_id();\n",
							cp->name);
			break;
		}
	}
	fprintf(fp, "\t}\n\treturn $r;\n}\n");

	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if (cp->sfname != NULL)
			genphpsearch(tp, cp, fp);
	}
	fprintf(fp, "}\n");
}
