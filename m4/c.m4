#line 1 "c.m4"
divert(-1)dnl
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
dnl Revision 1.3  2003/11/17 13:15:19  dtynan
dnl Various changes to fix errors in the back-end code.
dnl
dnl Revision 1.2  2003/10/14 14:10:56  dtynan
dnl Some fixes for SQL and C, as well as 'dnl' lines in the M4 templates to
dnl reduce blank lines in the output.
dnl
dnl Revision 1.1  2003/10/14 13:00:24  dtynan
dnl Major revision of the DBOW code to use M4 as a back-end instead of
dnl hard-coding the output.
dnl
define(`M4FILE',`c.m4')
define(`FILE_PROLOG',`define(`FILENAME',`$1')
/*
 * C/C++ prolog file - the following information was generated by
 * DBOW, the database object generator.  Do not edit the contents
 * of this file but instead the source file from which it was
 * generated.  Changes to this file will be lost.
 *
 * Source is "FILENAME"
 * Template is "$Id$"
 */
#include "dbow.h"
')
define(`FILE_EPILOG',`
')

define(`STYPE_UNSIGNED',	`unsigned')
define(`STYPE_TINYINT',		`short	$1')
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
define(`STYPE_CHAR',		`char	*$1')
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

define(`FTYPE_TINYINT',`')
define(`FTYPE_SMALLINT',`')
define(`FTYPE_MEDINT',`')
define(`FTYPE_INT',`')
define(`FTYPE_BIGINT',`')
define(`FTYPE_FLOAT',`')
define(`FTYPE_DOUBLE',`')
define(`FTYPE_NUMERIC',`')
define(`FTYPE_DATE',`')
define(`FTYPE_TIME',`')
define(`FTYPE_DATETIME',`')
define(`FTYPE_TSTAMP',`')
define(`FTYPE_YEAR',`')
define(`FTYPE_CHAR',`	if ($1 != NULL)
		free($1);
')
define(`FTYPE_VARCHAR',`	if ($1 != NULL)
		free($1);
')
define(`FTYPE_TINYBLOB',`	if ($1 != NULL)
		free($1);
')
define(`FTYPE_TINYTEXT',`	if ($1 != NULL)
		free($1);
')
define(`FTYPE_BLOB',`	if ($1 != NULL)
		free($1);
')
define(`FTYPE_TEXT',`	if ($1 != NULL)
		free($1);
')
define(`FTYPE_MEDBLOB',`	if ($1 != NULL)
		free($1);
')
define(`FTYPE_MEDTEXT',`	if ($1 != NULL)
		free($1);
')
define(`FTYPE_LONGBLOB',`	if ($1 != NULL)
		free($1);
')
define(`FTYPE_LONGTEXT',`	if ($1 != NULL)
		free($1);
')
define(`FTYPE_ENUM',`<error>')
define(`FTYPE_SET',`<error>')

define(ATYPE_TINYINT,`	$1 = ($2 == NULL) ? 0 : atoi($2);')
define(ATYPE_SMALLINT,`	$1 = ($2 == NULL) ? 0 : atoi($2);')
define(ATYPE_MEDINT,`	$1 = ($2 == NULL) ? 0 : atoi($2);')
define(ATYPE_INT,`	$1 = ($2 == NULL) ? 0 : atoi($2);')
define(ATYPE_BIGINT,`	$1 = ($2 == NULL) ? 0 : atol($2);')
define(ATYPE_FLOAT,`	$1 = ($2 == NULL) ? 0.0 : atof($2);')
define(ATYPE_DOUBLE,`	$1 = ($2 == NULL) ? 0.0 : strtod($2,(char **)NULL);')
define(ATYPE_NUMERIC,`	$1 = ($2 == NULL) ? 0 : atoi($2);')
define(ATYPE_DATE,`	dbow_parsedate(c,&$1,$2);')
define(ATYPE_TIME,`	dbow_parsedate(c,&$1,$2);')
define(ATYPE_DATETIME,`	dbow_parsedate(c,&$1,$2);')
define(ATYPE_TSTAMP,`	dbow_parsedate(c,&$1,$2);')
define(ATYPE_YEAR,`	$1 = ($2 == NULL) ? 0 : atoi($2);')
define(ATYPE_CHAR,`	if ($1 != NULL)
		free($1);
	$1 = ($2 == NULL) ? NULL : strdup($2);')
define(ATYPE_VARCHAR,`	if ($1 != NULL)
		free($1);
	$1 = ($2 == NULL) ? NULL : strdup($2);')
define(ATYPE_TINYBLOB,`	if ($1 != NULL)
		free($1);
	$1 = ($2 == NULL) ? NULL : strdup($2);')
define(ATYPE_TINYTEXT,`	if ($1 != NULL)
		free($1);
	$1 = ($2 == NULL) ? NULL : strdup($2);')
define(ATYPE_BLOB,`	if ($1 != NULL)
		free($1);
	$1 = ($2 == NULL) ? NULL : strdup($2);')
define(ATYPE_TEXT,`	if ($1 != NULL)
		free($1);
	$1 = ($2 == NULL) ? NULL : strdup($2);')
define(ATYPE_MEDBLOB,`	if ($1 != NULL)
		free($1);
	$1 = ($2 == NULL) ? NULL : strdup($2);')
define(ATYPE_MEDTEXT,`	if ($1 != NULL)
		free($1);
	$1 = ($2 == NULL) ? NULL : strdup($2);')
define(ATYPE_LONGBLOB,`	if ($1 != NULL)
		free($1);
	$1 = ($2 == NULL) ? NULL : strdup($2);')
define(ATYPE_LONGTEXT,`	if ($1 != NULL)
		free($1);
	$1 = ($2 == NULL) ? NULL : strdup($2);')
define(ATYPE_ENUM,`#error "ENUM type unsupported."')
define(ATYPE_SET,`#error "SET type unsupported."')

define(QTYPE_TINYINT,`$a')
define(QTYPE_SMALLINT,`$b')
define(QTYPE_MEDINT,`$c')
define(QTYPE_INT,`$d')
define(QTYPE_BIGINT,`$e')
define(QTYPE_FLOAT,`$f')
define(QTYPE_DOUBLE,`$g')
define(QTYPE_NUMERIC,`$h')
define(QTYPE_DATE,`$i')
define(QTYPE_TIME,`$j')
define(QTYPE_DATETIME,`$k')
define(QTYPE_TSTAMP,`$l')
define(QTYPE_YEAR,`$m')
define(QTYPE_CHAR,`$n')
define(QTYPE_VARCHAR,`$o')
define(QTYPE_TINYBLOB,`$p')
define(QTYPE_TINYTEXT,`$q')
define(QTYPE_BLOB,`$r')
define(QTYPE_TEXT,`$s')
define(QTYPE_MEDBLOB,`$t')
define(QTYPE_MEDTEXT,`$u')
define(QTYPE_LONGBLOB,`$v')
define(QTYPE_LONGTEXT,`$w')
define(QTYPE_ENUM,`$x')
define(QTYPE_SET,`$y')

define(PTYPE_TINYINT,`	fprintf(fp, "$1 = %d\n", $2);')
define(PTYPE_SMALLINT,`	fprintf(fp, "$1 = %d\n", $2);')
define(PTYPE_MEDINT,`	fprintf(fp, "$1 = %d\n", $2);')
define(PTYPE_INT,`	fprintf(fp, "$1 = %d\n", $2);')
define(PTYPE_BIGINT,`	fprintf(fp, "$1 = %ld\n", $2);')
define(PTYPE_FLOAT,`	fprintf(fp, "$1 = %f\n", $2);')
define(PTYPE_DOUBLE,`	fprintf(fp, "$1 = %lf\n", $2);')
define(PTYPE_NUMERIC,`	fprintf(fp, "$1 = ???\n");')
define(PTYPE_DATE,`	fprintf(fp, "$1 = %s", ctime(&$2));')
define(PTYPE_TIME,`	fprintf(fp, "$1 = %s", ctime(&$2));')
define(PTYPE_DATETIME,`	fprintf(fp, "$1 = %s", ctime(&$2));')
define(PTYPE_TSTAMP,`	fprintf(fp, "$1 = %s", ctime(&$2));')
define(PTYPE_YEAR,`	fprintf(fp, "$1 = %d\n", $2);')
define(PTYPE_CHAR,`	fprintf(fp, "$1 = \"%s\"\n", $2==NULL?"NULL":$2);')
define(PTYPE_VARCHAR,`	fprintf(fp, "$1 = \"%s\"\n", $2==NULL?"NULL":$2);')
define(PTYPE_TINYBLOB,`	fprintf(fp, "$1 = \"%s\"\n", $2==NULL?"NULL":$2);')
define(PTYPE_TINYTEXT,`	fprintf(fp, "$1 = \"%s\"\n", $2==NULL?"NULL":$2);')
define(PTYPE_BLOB,`	fprintf(fp, "$1 = \"%s\"\n", $2==NULL?"NULL":$2);')
define(PTYPE_TEXT,`	fprintf(fp, "$1 = \"%s\"\n", $2==NULL?"NULL":$2);')
define(PTYPE_MEDBLOB,`	fprintf(fp, "$1 = \"%s\"\n", $2==NULL?"NULL":$2);')
define(PTYPE_MEDTEXT,`	fprintf(fp, "$1 = \"%s\"\n", $2==NULL?"NULL":$2);')
define(PTYPE_LONGBLOB,`	fprintf(fp, "$1 = \"%s\"\n", $2==NULL?"NULL":$2);')
define(PTYPE_LONGTEXT,`	fprintf(fp, "$1 = \"%s\"\n", $2==NULL?"NULL":$2);')
define(PTYPE_ENUM,`	fprintf(fp, "$1 = ENUM?\n");')
define(PTYPE_SET,`	fprintf(fp, "$1 = SET?\n");')

define(`forloop',
       `pushdef(`$1', `$2')_forloop(`$1', `$2', `$3', `$4')popdef(`$1')')
define(`_forloop',
       `$4`'ifelse($1, `$3', ,
	   `define(`$1', incr($1))_forloop(`$1', `$2', `$3', `$4')')')
define(`concat',`$1$2')

define(STRNAME,`STR_$1_NAME$2')
define(STRTYPE,`STR_$1_TYPE$2')
define(STYPE,`concat(S,STR_$1_TYPE$2)')
define(QTYPE,`concat(Q,STR_$1_TYPE$2)')
define(FTYPE,`concat(F,STR_$1_TYPE$2)(p->STRNAME($1, $2))')
define(ATYPE,`concat(A,STR_$1_TYPE$2)(p->STRNAME($1, $2),row[$2])')
define(PTYPE,`concat(P,STR_$1_TYPE$2)(STRNAME($1, $2), p->STRNAME($1, $2))')

define(STRUCT_BODY,`
/*
 * Structure definition for SQL table "$1".
 */
struct db_$1 {
forloop(i,0,STR_$1_NELEM0,`	concat(STYPE($1,i),STRNAME($1,i));
')};')

define(INIT_PROTO,`
/*
 * Prototypes
 */
struct db_$1 *db_$1alloc();
void db_$1free(struct db_$1 *p);
struct db_$1 *db_find$1next(dbow_conn *c, struct db_$1 *p);')

define(INIT_BODY,`
struct db_$1 *
db_$1alloc()
{
	struct db_$1 *p;

	if ((p = (struct db_$1 *)malloc(sizeof(struct db_$1))) == NULL)
		return(NULL);
	memset((char *)p, 0, sizeof(struct db_$1));
	return(p);
}

void
db_$1free(struct db_$1 *p)
{
forloop(i,0,STR_$1_NELEM0,`FTYPE($1,i)')	free((char *)p);
}

struct db_$1 *
db_find$1next(dbow_conn *c, struct db_$1 *p)
{
	char **row;

	if (p == NULL && (p = db_$1alloc()) == NULL)
		return(NULL);
	if ((row = dbow_fetch_row(c)) == NULL) {
		db_$1free(p);
		return(NULL);
	}
forloop(i,0,STR_$1_NELEM0,`ATYPE($1,i)
')	return(p);
}')

define(INSERT_PROTO,`
int db_insert$1(dbow_conn *c, struct db_$1 *p);')

define(INSERT_BODY,`
int db_insert$1(dbow_conn *c, struct db_$1 *p)
{
	dbow_query(c, "INSERT INTO $1 VALUES (QTYPE($1,0) forloop(i,1,STR_$1_NELEM0,`,QTYPE($1,i)'))",
		p->STRNAME($1,0) forloop(i,1,STR_$1_NELEM0,`,p->STRNAME($1,i)'));
dnl
	if (p->STRNAME($1,0) == 0)
		p->STRNAME($1,0) = dbow_insertid(c);
	return(0);
}')

define(SEARCH_PROTO,`
struct db_$1 *concat(`db_find$1by',STRNAME($1,$2))(dbow_conn *c, STYPE($1, $2));')

define(SEARCH_BODY,`
struct db_$1 *
concat(`db_find$1by',STRNAME($1,$2))(dbow_conn *c, STYPE($1, $2) STRNAME($1,$2))
{
	if (dbow_query(c, "SELECT * FROM $1 WHERE STRNAME($1,$2) = QTYPE($1, $2)", STRNAME($1, $2)) < 0)
		return(NULL);
	return(db_find$1next(c, NULL));
}')

divert(0)dnl

define(DUMP_PROTO,`
void db_dump$1(struct db_$1 *, FILE *);')

define(DUMP_BODY,`
void
db_dump$1(struct db_$1 *p, FILE *fp)
{
forloop(i,0,STR_$1_NELEM0,`PTYPE($1,i)
')
}')
#include <string.h>
#include "dbow.h"

