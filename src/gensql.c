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
 * Revision 1.2  2004/01/26 23:43:21  dtynan
 * Extensive changes to fix some M4 issues and some library issues.
 * Removed many of the functions which were used to parse data types
 * and made them inline instead.  Improved the M4 generator by adding
 * for loops.
 *
 * Revision 1.1  2003/10/14 13:00:27  dtynan
 * Major revision of the DBOW code to use M4 as a back-end instead of
 * hard-coding the output.
 */

#include <stdio.h>

#include "dbowint.h"

char	*sqltypes[NTYPES] = {
	"tinyint", "smallint", "mediumint", "int", "bigint", "float", "double",
	"numeric", "date", "time", "datetime", "timestamp", "year", "char",
	"varchar", "tinyblob", "tinytext", "blob", "text", "mediumblob",
	"mediumtext", "longblob", "longtext", "enum", "set"
};

/*
 *
 */
void
generatesql(struct table *tp, FILE *fp)
{
	int havepk;
	struct column *cp;

	fprintf(fp, "--\n-- Table structure for table `%s`\n--\n\n", tp->name);
	fprintf(fp, "DROP TABLE IF EXISTS %s;\n", tp->name);
	fprintf(fp, "CREATE TABLE %s (\n", tp->name);
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		fprintf(fp, "  %s %s", cp->name, sqltypes[cp->type]);
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
		if (cp->flags & FLAG_UNIQUE)
			fprintf(fp, " UNIQUE");
		if (cp->flags & FLAG_AUTOINC)
			fprintf(fp, " auto_increment");
		else {
			fprintf(fp, " default ");
			if (cp->flags & FLAG_NOTNULL) {
				switch (cp->type) {
				case TYPE_TINYINT:
				case TYPE_SMALLINT:
				case TYPE_MEDINT:
				case TYPE_INT:
				case TYPE_BIGINT:
				case TYPE_FLOAT:
				case TYPE_DOUBLE:
				case TYPE_NUMERIC:
					fprintf(fp, "'0'");
					break;

				case TYPE_DATE:
				case TYPE_TIME:
				case TYPE_DATETIME:
				case TYPE_TSTAMP:
				case TYPE_YEAR:
					fprintf(fp, "'0000-00-00'");
					break;

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
					fprintf(fp, "''");
					break;
				}
			} else
				fprintf(fp, "NULL");
		}
		fprintf(fp, ",\n");
	}
	havepk = 0;
	for (cp = tp->chead; cp != NULL; cp = cp->next) {
		if ((cp->flags & FLAG_PRIKEY) == 0)
			continue;
		if (havepk == 0) {
			fprintf(fp, "  PRIMARY KEY  (%s", cp->name);
			havepk = 1;
		} else {
			fprintf(fp, ", %s", cp->name);
		}
	}
	if (havepk)
		fprintf(fp, ")\n");
	fprintf(fp, ") TYPE=MyISAM;\n\n");
}
