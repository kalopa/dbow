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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>

#ifdef DBOW_MYSQL
#include "mysql.h"
#endif

#include "dbow.h"
#include "../src/dbowint.h"

#define QBCHUNK	4096

/*
 *
 */
static int
qbputc(dbow_conn *c, int ch)
{
	if (c->qbuff == NULL || c->qboff >= c->qbsize) {
		char *nqbp;

		if (c->qbuff == NULL)
			c->qboff = c->qbsize = 0;
		c->qbsize += QBCHUNK;
		nqbp = realloc(c->qbuff, c->qbsize);
		if (nqbp == NULL || c->qboff > 65000) {
			if (c->qbuff != NULL)
				free(c->qbuff);
			c->qbuff = NULL;
			return(-1);
		}
		c->qbuff = nqbp;
	}
	c->qbuff[c->qboff++] = ch;
	return(c->qboff);
}

/*
 *
 */
static int
qbputs(dbow_conn *c, char *cp)
{
	int n = 0;

	while (*cp && n >= 0)
		n = qbputc(c, *cp++);
	return(n);
}

/*
 *
 */
int
dbow_query(dbow_conn *c, char *query, ...)
{
	int n, ch, state, val;
	char obuf[32], *cp;
	struct tm *tmp;
	va_list ap;

	va_start(ap, query);
	state = n = 0;
	while (n >= 0 && (ch = *query++) != '\0') {
		if (state == 0) {
			if (ch == '$')
				state = 1;
			else
				n = qbputc(c, ch);
			continue;
		}
		if (ch < 'a' || ch > 'z') {
			qbputc(c, ch);
			state = 0;
			continue;
		}
		state = n = 0;
		cp = NULL;
		switch (ch - 'a') {
		case TYPE_TINYINT: case TYPE_SMALLINT: case TYPE_MEDINT:
		case TYPE_INT: case TYPE_YEAR:
			sprintf(obuf, "%d", va_arg(ap, int));
			n = qbputs(c, obuf);
			break;

		case TYPE_BIGINT:
			sprintf(obuf, "%ld", va_arg(ap, long));
			n = qbputs(c, obuf);
			break;

		case TYPE_FLOAT: case TYPE_DOUBLE:
			sprintf(obuf, "%lf", va_arg(ap, double));
			n = qbputs(c, obuf);
			break;

		case TYPE_CHAR: case TYPE_VARCHAR: case TYPE_TINYBLOB:
		case TYPE_TINYTEXT: case TYPE_BLOB: case TYPE_TEXT:
		case TYPE_MEDBLOB: case TYPE_MEDTEXT: case TYPE_LONGBLOB:
		case TYPE_LONGTEXT:
			if ((cp = va_arg(ap, char *)) == NULL) {
				n = qbputs(c, "NULL");
				break;
			}
			n = qbputc(c, '\'');
			while ((ch = *cp++) != '\0' && n >= 0) {
				switch (ch) {
				case '\'':
				case '\\':
					qbputc(c, '\\');
					n = qbputc(c, ch);
					break;

				default:
					n = qbputc(c, ch);
					break;
				}
			}
			if (n >= 0)
				n = qbputc(c, '\'');
			break;

		case TYPE_DATE:
			val = va_arg(ap, int);
			if (val == 0)
				n = qbputs(c, "NULL");
			else {
				if ((tmp = localtime((time_t *)&val)) == NULL)
					return(-1);
				sprintf(obuf, "'%04d-%02d-%02d'",
					tmp->tm_year + 1900,
					tmp->tm_mon + 1,
					tmp->tm_mday);
				n = qbputs(c, obuf);
			}
			break;

		case TYPE_TIME:
			val = va_arg(ap, int);
			if (val == 0)
				n = qbputs(c, "NULL");
			else {
				if ((tmp = localtime((time_t *)&val)) == NULL)
					return(-1);
				sprintf(obuf, "'%02d:%02d:%02d'",
					tmp->tm_hour,
					tmp->tm_min,
					tmp->tm_sec);
				n = qbputs(c, obuf);
			}
			break;

		case TYPE_DATETIME:
			val = va_arg(ap, int);
			if (val == 0)
				n = qbputs(c, "NULL");
			else {
				if ((tmp = localtime((time_t *)&val)) == NULL)
					return(-1);
				sprintf(obuf, "'%04d-%02d-%02d %02d:%02d:%02d'",
					tmp->tm_year + 1900,
					tmp->tm_mon + 1,
					tmp->tm_mday,
					tmp->tm_hour,
					tmp->tm_min,
					tmp->tm_sec);
				n = qbputs(c, obuf);
			}
			break;

		case TYPE_TSTAMP:
			val = va_arg(ap, int);
			if (val == 0)
				n = qbputs(c, "NULL");
			else {
				if ((tmp = localtime((time_t *)&val)) == NULL)
					return(-1);
				sprintf(obuf, "'%04d%02d%02d%02d%02d%02d'",
					tmp->tm_year + 1900,
					tmp->tm_mon + 1,
					tmp->tm_mday,
					tmp->tm_hour,
					tmp->tm_min,
					tmp->tm_sec);
				n = qbputs(c, obuf);
			}
			break;

		case TYPE_NUMERIC: case TYPE_ENUM: case TYPE_SET:
			n = -1;
			break;
		}
	}
	va_end(ap);
	if (n < 0 || (n = qbputc(c, '\0')) < 0)
		return(-1);
	c->qboff = 0;
#ifdef DBOW_MYSQL
	if (c->trace)
		printf("[%s]\n", c->qbuff);
	if (mysql_query(c->dbconn, c->qbuff) < 0)
		return(-1);
	c->dbres = (void *)mysql_store_result((MYSQL *)c->dbconn);
	return(0);
#endif
	return(-1);
}

/*
 *
 */
char **
dbow_fetch_row(void *vp)
{
#ifdef DBOW_MYSQL
	MYSQL_RES *dbres = (MYSQL_RES *)vp;

	if (dbres != NULL)
		return((char **)mysql_fetch_row(dbres));
#endif
	return(NULL);
}

/*
 *
 */
void
dbow_free_result(void *vp)
{
#ifdef DBOW_MYSQL
	MYSQL_RES *dbres = (MYSQL_RES *)vp;

	if (dbres != NULL)
		mysql_free_result(dbres);
#endif
}
