divert(-1)dnl
dnl
dnl Copyright (c) 2003, Kalopa Media Limited.
dnl Copyright (c) 2015, Kalopa Research.
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
dnl THIS SOFTWARE IS PROVIDED BY KALOPA RESEARCH "AS IS" AND ANY
dnl EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
dnl THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
dnl PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA
dnl RESEARCH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
dnl EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
dnl TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
dnl DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
dnl ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
dnl OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
dnl OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
dnl
define(`M4FILE',`php.m4')
define(`FILE_PROLOG',`define(`FILENAME',`$1')
/*
 * PHP prolog file - the following information was generated by
 * DBOW, the database object generator.  Do not edit the contents
 * of this file but instead the source file from which it was
 * generated.  Changes to this file will be lost.
 *
 * Source is "FILENAME"
 */
')
define(`FILE_EPILOG',`
}
')

define(`STYPE_UNSIGNED',	`unsigned')
define(`STYPE_TINYINT',		`var	$$1')
define(`STYPE_SMALLINT',	`var	$$1')
define(`STYPE_MEDINT',		`var	$$1')
define(`STYPE_INT',		`var	$$1')
define(`STYPE_BIGINT',		`var	$$1')
define(`STYPE_FLOAT',		`var	$$1')
define(`STYPE_DOUBLE',		`var	$$1')
define(`STYPE_NUMERIC',		`var	$$1')
define(`STYPE_DATE',		`var	$$1')
define(`STYPE_TIME',		`var	$$1')
define(`STYPE_DATETIME',	`var	$$1')
define(`STYPE_TSTAMP',		`var	$$1')
define(`STYPE_YEAR',		`var	$$1')
define(`STYPE_CHAR',		`var	$$1')
define(`STYPE_VARCHAR',		`var	$$1')
define(`STYPE_TINYBLOB',	`var	$$1')
define(`STYPE_TINYTEXT',	`var	$$1')
define(`STYPE_BLOB',		`var	$$1')
define(`STYPE_TEXT',		`var	$$1')
define(`STYPE_MEDBLOB',		`var	$$1')
define(`STYPE_MEDTEXT',		`var	$$1')
define(`STYPE_LONGBLOB',	`var	$$1')
define(`STYPE_LONGTEXT',	`var	$$1')
define(`STYPE_ENUM',		`<error>')
define(`STYPE_SET',		`<error>')

define(PTYPE_TINYINT,`	echo "$1 = $2<br>\n");')
define(PTYPE_SMALLINT,`	echo "$1 = $2<br>\n");')
define(PTYPE_MEDINT,`	echo "$1 = $2<br>\n");')
define(PTYPE_INT,`	echo "$1 = $2<br>\n");')
define(PTYPE_BIGINT,`	echo "$1 = $2<br>\n");')
define(PTYPE_FLOAT,`	echo "$1 = $2<br>\n");')
define(PTYPE_DOUBLE,`	echo "$1 = $2<br>\n");')
define(PTYPE_NUMERIC,`	echo "$1 = ???\n");')
define(PTYPE_DATE,`	echo "$1 = $2<br>\n");')
define(PTYPE_TIME,`	echo "$1 = $2<br>\n");')
define(PTYPE_DATETIME,`	echo "$1 = $2<br>\n");')
define(PTYPE_TSTAMP,`	echo "$1 = $2<br>\n");')
define(PTYPE_YEAR,`	echo "$1 = $2<br>\n");')
define(PTYPE_CHAR,`	echo "$1 = $2<br>\n");')
define(PTYPE_VARCHAR,`	echo "$1 = $2<br>\n");')
define(PTYPE_TINYBLOB,`	echo "$1 = $2<br>\n");')
define(PTYPE_TINYTEXT,`	echo "$1 = $2<br>\n");')
define(PTYPE_BLOB,`	echo "$1 = $2<br>\n");')
define(PTYPE_TEXT,`	echo "$1 = $2<br>\n");')
define(PTYPE_MEDBLOB,`	echo "$1 = $2<br>\n");')
define(PTYPE_MEDTEXT,`	echo "$1 = $2<br>\n");')
define(PTYPE_LONGBLOB,`	echo "$1 = $2<br>\n");')
define(PTYPE_LONGTEXT,`	echo "$1 = $2<br>\n");')
define(PTYPE_ENUM,`	echo "$1 = ENUM?\n");')
define(PTYPE_SET,`	echo "$1 = SET?\n");')

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
 * Class definition for SQL table "$1".
 */
class $1 {
	var	$db;
forloop(i,0,STR_$1_NELEM0,`	concat(STYPE($1,i),STRNAME($1,i));
')
function getdb() { return($this->db); }
function setdb($val) { $this->db = $val; }
forloop(i,0,STR_$1_NELEM0,`function concat(get,STRNAME($1,i))() { return($this->STRNAME($1,i)); }
function concat(set,STRNAME($1,i))($val) { $this->STRNAME($1,i) = $val; }
')
')

define(INIT_PROTO,`')dnl
define(INSERT_PROTO,`')dnl
define(DELETE_PROTO,`')dnl
define(SEARCH_PROTO,`')dnl
define(UPDATE_PROTO,`')dnl
define(DUMP_PROTO,`')dnl

define(INIT_BODY,`
function setfields($r)
{
	$e = mysql_fetch_array($r);
forloop(i,0,STR_$1_NELEM0,`	$this->concat(set,STRNAME($1,i))($e[i]);
')}')

define(INSERT_BODY,`
function $2()
{
	if ($this->db->mysql_query("INSERT INTO $1 VALUES($STRNAME($1,0) forloop(i,1,STR_$1_NELEM0,`,$STRNAME($1,i)'))"))
		return $TRUE;
	else
		return $FALSE;
}')

define(DELETE_BODY,`
function $2($x)
{
	if ($this->db->mysql_query("DELETE FROM $1 WHERE STRNAME($1,$3)=$x")) {
		return $TRUE;
	else
		return $FALSE;
}')

define(SEARCH_BODY,`
function $2($x)
{
	if (!$r = $this->db->mysql_query("SELECT STRNAME($1,0) forloop(z,1,STR_$1_NELEM0,`,STRNAME($1,z)') FROM $1 WHERE STRNAME($1,$3)=$x")) {
		$r = $FALSE;
	} else {
		$this->setfields($r);
	}
	return($r);
}')

define(UPDATE_BODY,`
function $2()
{
	if ($this->db->mysql_query("UPDATE $1 SET (STRNAME($1,0)=$STRNAME($1,0) forloop(i,1,STR_$1_NELEM0,`,STRNAME($1,i)=$STRNAME($1,i)'))"))
		return $TRUE;
	else
		return $FALSE;
}')

define(DUMP_BODY,`
function $2()
{
forloop(i,0,STR_$1_NELEM0,`PTYPE($1,i)
')
}')

divert(0)dnl
