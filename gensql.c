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

#include "dbowint.h"

char	*sqltypes[NTYPES] = {
	"tinyint", "smallint", "mediumint", "int", "bigint", "float", "double",
	"numeric", "date", "time", "datetime", "timestamp", "year", "char",
	"varchar", "tinyblob", "tinytext", "blob", "text", "mediumblob",
	"mediumtext", "longblob", "longtext", "", ""
};

/*
 *
 */
int
str_sql(struct table *tp, FILE *fp)
{
	struct column *cp;

	printf("Generate SQL for table [%s]\n", tp->name);
	fprintf(fp, "\nDROP TABLE IF EXISTS %s;\n", tp->name);
	fprintf(fp, "\nCREATE TABLE %s\n(\n", tp->name);
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		fprintf(fp, "\t%s %s", cp->name, sqltypes[cp->type]);
		if (cp->length > 0) {
			fprintf(fp, "(%d", cp->length);
			if (cp->dprec > 0)
				fprintf(fp, ",%d", cp->dprec);
			fputc(')', fp);
		}
		if (cp->flags & FLAG_UNSIGNED)
			fprintf(fp, " UNSIGNED");
		if (cp->flags & FLAG_NOTNULL)
			fprintf(fp, " NOT NULL");
		if (cp->flags & FLAG_PRIKEY)
			fprintf(fp, " PRIMARY KEY");
		if (cp->flags & FLAG_AUTOINC)
			fprintf(fp, " AUTO_INCREMENT");
		if (cp->next != NULL)
			fputc(',', fp);
		fputc('\n', fp);
	}
	fprintf(fp, ");\n");
}
