/*
 * Copyright (c) 2003, Kalopa Media Limited.
 * Copyright (c) 2015-22, Kalopa Robotics Limited.
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
 * THIS SOFTWARE IS PROVIDED BY KALOPA ROBOTICS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA
 * ROBOTICS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dbowint.h"
#include "dbow.h"

char	*sqltypes[NTYPES] = {
	"tinyint", "smallint", "mediumint", "int", "bigint", "float", "double",
	"numeric", "date", "time", "datetime", "timestamp", "year", "char",
	"varchar", "tinyblob", "tinytext", "blob", "text", "mediumblob",
	"mediumtext", "longblob", "longtext", "enum", "set"
};

char	buffer[4096];

/*
 *
 */
void
prtablename(char *xp, char *tname, char *alias)
{
	sprintf(xp, "%s", tname);
	if (alias != NULL) {
		xp = strchr(xp, '\0');
		sprintf(xp, " %s", alias);
	}
}

/*
 *
 */
void
dosetcols(char *xp, struct column *cp)
{
	int argno = 0;

	strcpy(xp, " SET ");
	xp += 5;
	while (cp != NULL) {
		if (argno++ > 0)
			*xp++ = ',';
		sprintf(xp, "%s=$%d", cp->name, argno);
		xp = strchr(xp, '\0');
		cp = cp->next;
	}
}

/*
 *
 */
void
doselectcols(char *xp, struct column *cp, char *prefix)
{
	int first = 1;

	while (cp != NULL) {
		if (!first)
			*xp++ = ',';
		first = 0;
		if (prefix)
			sprintf(xp, "%s.%s", prefix, cp->name);
		else
			sprintf(xp, "%s", cp->name);
		cp = cp->next;
		xp = strchr(xp, '\0');
	}
}

/*
 *
 */
void
buildinsertsql(struct func *fp)
{
	char *xp;

	strcpy(buffer, "INSERT INTO ");
	xp = &buffer[12];
	prtablename(xp, fp->table->name, fp->alias);
	xp = strchr(buffer, '\0');
	dosetcols(xp, fp->table->chead);
	fp->query = strdup(buffer);
}

/*
 *
 */
void
builddeletesql(struct func *fp)
{
	char *xp;

	strcpy(buffer, "DELETE FROM ");
	xp = &buffer[12];
	prtablename(xp, fp->table->name, fp->alias);
	xp = strchr(buffer, '\0');
	if (fp->where != NULL)
		sprintf(xp, " WHERE %s", fp->where);
	fp->query = strdup(buffer);
}

/*
 *
 */
void
buildsearchsql(struct func *fp)
{
	char *xp;
	struct join *jp;

	sprintf(buffer, "SELECT ");
	xp = strchr(buffer, '\0');
	if (fp->joins != NULL) {
		if (fp->alias != NULL)
			doselectcols(xp, fp->table->chead, fp->alias);
		else
			doselectcols(xp, fp->table->chead, fp->table->name);
	} else
		doselectcols(xp, fp->table->chead, fp->alias);
	strcat(buffer, " FROM ");
	xp = strchr(buffer, '\0');
	prtablename(xp, fp->table->name, fp->alias);
	xp = strchr(buffer, '\0');
	/*
	 * Do joins...
	 */
	for (jp = fp->joins; jp != NULL; jp = jp->next) {
		*xp++ = ',';
		prtablename(xp, jp->table->name, jp->alias);
		xp = strchr(buffer, '\0');
	}
	/*
	 * Where clause...
	 */
	if (fp->where != NULL) {
		sprintf(xp, " WHERE %s", fp->where);
		xp = strchr(buffer, '\0');
	}
	/*
	 * Order by clause...
	 */
	if (fp->order != NULL) {
		sprintf(xp, " ORDER BY %s", fp->order);
		xp = strchr(buffer, '\0');
	}
	/*
	 * Limit clause...
	 */
	if (fp->limit > 0) {
		if (fp->offset > 0)
			sprintf(xp, " LIMIT %d,%d", fp->offset, fp->limit);
		else
			sprintf(xp, " LIMIT %d", fp->limit);
	} else if (fp->offset > 0)
		sprintf(xp, " LIMIT %d,999999999999", fp->offset);
	fp->query = strdup(buffer);
}

/*
 *
 */
void
buildupdatesql(struct func *fp)
{
	char *xp;

	strcpy(buffer, "UPDATE ");
	xp = &buffer[7];
	prtablename(xp, fp->table->name, fp->alias);
	xp = strchr(xp, '\0');
	dosetcols(xp, fp->table->chead);
	xp = strchr(xp, '\0');
	if (fp->where != NULL)
		sprintf(xp, " WHERE %s", fp->where);
	fp->query = strdup(buffer);
}

/*
 *
 */
void
buildwhereclause(struct func *fp)
{
	int argno = 0;
	char *xp;
	struct arg *ap, *atail;
	struct column *cp;

	if (fp->type != DBOW_SEARCH && fp->type != DBOW_UPDATE && \
						fp->type != DBOW_DELETE)
		return;
	fp->args = NULL;
	if (fp->pkey != NULL) {
		sprintf(buffer, "%s=$1", fp->pkey->name);
		fp->where = strdup(buffer);
		fp->args = newarg(fp->table, fp->pkey->name);
	} else {
		xp = buffer;
		for (cp = fp->table->chead; cp != NULL; cp = cp->next) {
			if ((cp->flags & FLAG_PRIKEY) == 0)
				continue;
			if (argno++ > 0) {
				strcpy(xp, " AND ");
				xp += 5;
			}
			sprintf(xp, "%s=$%d", cp->name, argno);
			xp = strchr(xp, '\0');
			ap = newarg(fp->table, cp->name);
			if (fp->args == NULL)
				fp->args = ap;
			else
				atail->next = ap;
			atail = ap;
		}
		*xp = '\0';
		fp->where = strdup(buffer);
	}
}

/*
 *
 */
void
translatesql(struct func *fp)
{
	int state, push, argno;
	char *cp, *cp2;
	struct arg *ap;

	/*
	 * Run through the SQL and convert the arg callouts to special
	 * place-holders for dbow_query to rewrite.
	 */
	printf("BEFORE: [%s]\n", fp->query);
	for (state = 0, cp = fp->query; *cp; cp++) {
		switch (state) {
		case 0:
			if (*cp == '"' || *cp == '\'') {
				push = *cp;
				state = 1;
			} else if (*cp == '$') {
				cp2 = cp;
				argno = 0;
				state = 2;
			}
			break;

		case 1:
			if (*cp == push)
				state = 0;
			break;

		case 2:
			if (isdigit(*cp)) {
				argno = argno * 10 + *cp - '0';
				break;
			}
			/*
			 * Found termination.
			 */
#if 0
			ap = findarg(fp, argno);
			ap = fp->args;
			while (--argno > 0)
				if (ap->next != NULL)
					ap = ap->next;
			printf("Arg: %d (%s)\n", argno, ap->column->name);
#endif
			state = 0;
			break;
		}
	}
	printf("AFTER: [%s]\n", fp->query);
}

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
