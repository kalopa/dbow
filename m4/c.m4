dnl
dnl $Id$
dnl
dnl Copyright (c) 2003, Kalopa Media Limited.  All rights reserved.
dnl
dnl This is free software; you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2, or (at your option)
dnl any later version.
dnl
dnl It is distributed in the hope that it will be useful, but WITHOUT
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
dnl License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this product; see the file COPYING.  If not, write to
dnl the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139,
dnl USA.
dnl
dnl THIS SOFTWARE IS PROVIDED BY KALOPA MEDIA LIMITED "AS IS" AND ANY
dnl EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
dnl IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
dnl PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA MEDIA LIMITED BE
dnl LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
dnl OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
dnl OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
dnl OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
dnl LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
dnl NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
dnl SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
dnl
dnl ABSTRACT
dnl
dnl $Log$
dnl
define(`LSYNC',`#line $1 "$2"')
define(`M4FILE',`c.m4')
LSYNC(__line__,M4FILE)
define(`FILE_PROLOG',`define(`FILENAME',`$1')
LSYNC(1,FILENAME)
LSYNC(__line__,M4FILE)
/*
 * C/C++ prolog file - the following information was generated by
 * DBOW, the database object generator.  Do not edit the contents
 * of this file but instead the source file from which it was
 * generated.  Changes to this file will be lost.
 *
 * Source is "FILENAME"
 * Template is "$Id$"
 */
')
define(`FILE_EPILOG',`
')
define(`INCLUDE',`
#include <$1>')

define(`STYPE_UNSIGNED',	`unsigned')
define(`STYPE_TINYINT',		`char	$1')
define(`STYPE_SMALLINT',	`short	$1')
define(`STYPE_MEDINT',		`int	$1')
define(`STYPE_INT',		`int	$1')
define(`STYPE_BIGINT',		`long	$1')
define(`STYPE_FLOAT',		`float	$1')
define(`STYPE_DOUBLE',		`double	$1')
define(`STYPE_NUMERIC',		`char	*$1')
define(`STYPE_DATE',		`int	$1')
define(`STYPE_TIME',		`int	$1')
define(`STYPE_DATETIME',	`int	$1')
define(`STYPE_TSTAMP',		`int	$1')
define(`STYPE_YEAR',		`int	$1')
define(`STYPE_CHAR',		`char	$1[$2]')
define(`STYPE_VARCHAR',		`char	*$1')
define(`STYPE_TINYBLOB',	`char	*$1')
define(`STYPE_TINYTEXT',	`char	*$1')
define(`STYPE_BLOB',		`char	*$1')
define(`STYPE_TEXT',		`char	*$1')
define(`STYPE_MEDBLOB',		`char	*$1')
define(`STYPE_MEDTEXT',		`char	*$1')
define(`STYPE_LONGBLOB',	`char	*$1')
define(`STYPE_LONGTEXT',	`char	*$1')
define(`STYPE_ENUM',		`<error>')
define(`STYPE_SET',		`<error>')

define(`FTYPE_TINYINT',		`int')
define(`FTYPE_SMALLINT',	`int')
define(`FTYPE_MEDINT',		`int')
define(`FTYPE_INT',		`int')
define(`FTYPE_BIGINT',		`long')
define(`FTYPE_FLOAT',		`float')
define(`FTYPE_DOUBLE',		`double')
define(`FTYPE_NUMERIC',		`dchar')
define(`FTYPE_DATE',		`date')
define(`FTYPE_TIME',		`date')
define(`FTYPE_DATETIME',	`date')
define(`FTYPE_TSTAMP',		`date')
define(`FTYPE_YEAR',		`int')
define(`FTYPE_CHAR',		`char')
define(`FTYPE_VARCHAR',		`char')
define(`FTYPE_TINYBLOB',	`char')
define(`FTYPE_TINYTEXT',	`char')
define(`FTYPE_BLOB',		`char')
define(`FTYPE_TEXT',		`char')
define(`FTYPE_MEDBLOB',		`char')
define(`FTYPE_MEDTEXT',		`char')
define(`FTYPE_LONGBLOB',	`char')
define(`FTYPE_LONGTEXT',	`char')
define(`FTYPE_ENUM',		`<error>')
define(`FTYPE_SET',		`<error>')

define(`STR_PROLOG',`
/*
 * Structure definition for SQL table "$1".
 */
struct db_$1 {
	dbow_res *resp;')
define(`STR_ELEMENT',`	$1;')
define(`STR_EPILOG',`};')

define(`PROTO_PROLOG',`
/*
 * Prototypes
 */
struct db_$1 *db_$1alloc();
void db_$1free(struct db_$1 *);
struct db_$1 *db_find$1next(struct db_$1 *);
')
define(`PROTO_INSERT',`int $2(dbow_conn *, struct db_$1 *);')
define(`PROTO_SEARCH',`struct db_$1 *$2(dbow_conn *, $3);')
define(`PROTO_DUMP',`void $2(struct db_$1 *, FILE *);')
define(`PROTO_EPILOG',`')

define(`FILL_PROLOG',`
static void
_dbfill_$1(dbow_row row, struct db_$1 *p)
{')
define(`FILL_ELEMENT',`	dbow_free$2(p->$1);
	p->$1 = dbow_f$2(row, $3);')
define(`FILL_EPILOG',`}')

define(`ALLOC_PROLOG',`
struct db_$1 *
db_$1alloc()
{
	struct db_$1 *p;

	if ((p = (struct db_$1 *)dbow_alloc(sizeof(struct db_$1))) == NULL)
		return(NULL);
	memset((char *)p, 0, sizeof(struct db_$1));
	return(p);')
define(`ALLOC_ELEMENT',`')
define(`ALLOC_EPILOG',`}')

define(`FREE_PROLOG',`
void
db_$1free(struct db_$1 *p)
{')
define(`FREE_ELEMENT',`	dbow_free$2(p->$1);')
define(`FREE_EPILOG',`	dbow_free((char *)p);
}')

define(`INSERT_PROLOG',`
int
$2(dbow_conn *c, struct db_$1 *p)
{
	char q[1024];
	MYSQL_RES *rsp;

	strcpy(q, "INSERT INTO $1 VALUES(");')
define(`INSERT_ELEMENT',`	if (dbow_i$2(DBOW_INSERT,q,p->$1,sizeof(q)) < 0)
		return(-1);')
define(`INSERT_ACTION',`	if (dbow_query(c,q) != 0)
		return(-1);')
define(`INSERT_SETID',`	if (p->$1 == 0)
		p->$1 = dbow_insertid(c);')
define(`INSERT_EPILOG',` return(0);
}')

define(`SEARCH_NEXT',`
struct db_$1 *
db_find$1next(struct db_$1 *p)
{
	dbow_row row;

	if (p->resp == NULL)
		return(NULL);
	if ((row = dbow_fetch_row(p->resp)) == NULL) {
		dbow_free_result(p->resp);
		p->resp = NULL;
		return(NULL);
	}
	_dbfill_$1(row, p);
	return(p);
}
define(`HAVE_SEARCH_NEXT_$1')')
define(`SEARCH_PROLOG',`ifdef(`HAVE_SEARCH_NEXT_$1',`',`SEARCH_NEXT($1)')
struct db_$1 *
$2(dbow_conn *c, $3)
{
	struct db_$1 *p = NULL;
	char q[100];
	dbow_row row;

	strcpy(q, "SELECT * FROM $1 WHERE");
')
define(`SEARCH_ELEMENT',`	if (dbow_ichar(DBOW_OTHER,q,"$1 = ",sizeof(q)) < 0 || dbow_i$2(DBOW_SEARCH,q,$1,sizeof(q)) < 0)
		return(NULL);')
define(`SEARCH_ACTION',`
	if (dbow_query(c, q) != 0)
		return(NULL);
	if ((p->resp = dbow_store_result(c)) == NULL)
		return(NULL);')
define(`SEARCH_EPILOG',`
	return(db_find$1next(p));
}')

define(`DUMP_PROLOG',`
void
$2(struct db_$1 *p, FILE *fp)
{
	fprintf(fp, "Dump of record from table $1:-\n");
')
define(`DUMP_ELEMENT',`	dbow_dump$2(fp, p->$1);')
define(`DUMP_EPILOG',`}')
