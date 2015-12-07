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
define(`LSYNC',`')
define(`M4FILE',`mysql.m4')
define(`FILE_PROLOG',`define(`FILENAME',`$1')
#
# MySQL prolog file - the following information was generated by
# DBOW, the database object generator.  Do not edit the contents
# of this file but instead the source file from which it was
# generated.  Changes to this file will be lost.
#
# Source is "FILENAME"
#
')
define(`FILE_EPILOG',`
')
define(`INCLUDE',`')

define(`GEN_STYPE', `$1($2)')
define(`STYPE_UNSIGNED',	`unsigned')
define(`STYPE_TINYINT',		`GEN_STYPE(`$1 tinyint', $2)')
define(`STYPE_SMALLINT',	`GEN_STYPE(`$1 smallint', $2)')
define(`STYPE_MEDINT',		`GEN_STYPE(`$1 mediumint', $2)')
define(`STYPE_INT',		`GEN_STYPE(`$1 int', $2)')
define(`STYPE_BIGINT',		`GEN_STYPE(`$1 bigint', $2)')
define(`STYPE_FLOAT',		`GEN_STYPE(`$1 float', $2)')
define(`STYPE_DOUBLE',		`GEN_STYPE(`$1 double', $2)')
define(`STYPE_NUMERIC',		`GEN_STYPE(`$1 numeric', $2)')
define(`STYPE_DATE',		`GEN_STYPE(`$1 date', $2)')
define(`STYPE_TIME',		`GEN_STYPE(`$1 time', $2)')
define(`STYPE_DATETIME',	`GEN_STYPE(`$1 datetime', $2)')
define(`STYPE_TSTAMP',		`GEN_STYPE(`$1 timestamp', $2)')
define(`STYPE_YEAR',		`GEN_STYPE(`$1 year', $2)')
define(`STYPE_CHAR',		`GEN_STYPE(`$1 char', $2)')
define(`STYPE_VARCHAR',		`GEN_STYPE(`$1 varchar', $2)')
define(`STYPE_TINYBLOB',	`GEN_STYPE(`$1 tinyblob', $2)')
define(`STYPE_TINYTEXT',	`GEN_STYPE(`$1 tinytext', $2)')
define(`STYPE_BLOB',		`GEN_STYPE(`$1 blob', $2)')
define(`STYPE_TEXT',		`GEN_STYPE(`$1 text', $2)')
define(`STYPE_MEDBLOB',		`GEN_STYPE(`$1 mediumblob', $2)')
define(`STYPE_MEDTEXT',		`GEN_STYPE(`$1 mediumtext', $2)')
define(`STYPE_LONGBLOB',	`GEN_STYPE(`$1 longblob', $2)')
define(`STYPE_LONGTEXT',	`GEN_STYPE(`$1 longtext', $2)')
define(`STYPE_ENUM',		`GEN_STYPE(`$1 enum', $2)')
define(`STYPE_SET',		`GEN_STYPE(`$1 set', $2)')

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
define(`FTYPE_CHAR',`')
define(`FTYPE_VARCHAR',`')
define(`FTYPE_TINYBLOB',`')
define(`FTYPE_TINYTEXT',`')
define(`FTYPE_BLOB',`')
define(`FTYPE_TEXT',`')
define(`FTYPE_MEDBLOB',`')
define(`FTYPE_MEDTEXT',`')
define(`FTYPE_LONGBLOB',`')
define(`FTYPE_LONGTEXT',`')
define(`FTYPE_ENUM',`')
define(`FTYPE_SET',`')

define(`STR_PROLOG',`

DROP TABLE IF EXISTS $1;

CREATE TABLE $1
(')
define(`STR_ELEMENT',`	$1,')
define(`STR_EPILOG',`);')

define(`PROTO_PROLOG',`')
define(`PROTO_INSERT',`')
define(`PROTO_SEARCH',`')
define(`PROTO_DUMP',`')
define(`PROTO_EPILOG',`')

define(`FILL_PROLOG',`')
define(`FILL_ELEMENT',`')
define(`FILL_EPILOG',`')

define(`ALLOC_PROLOG',`')
define(`ALLOC_ELEMENT',`')
define(`ALLOC_EPILOG',`')

define(`FREE_PROLOG',`')
define(`FREE_ELEMENT',`')
define(`FREE_EPILOG',`')

define(`INSERT_PROLOG',`')
define(`INSERT_ELEMENT',`')
define(`INSERT_ACTION',`')
define(`INSERT_SETID',`')
define(`INSERT_EPILOG',`')

define(`SEARCH_PROLOG',`')
define(`SEARCH_ELEMENT',`')
define(`SEARCH_ACTION',`')
define(`SEARCH_EPILOG',`')

define(`DUMP_PROLOG',`')
define(`DUMP_ELEMENT',`')
define(`DUMP_EPILOG',`')
