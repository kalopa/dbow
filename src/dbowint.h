/*
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
 */

#define DBASE_MYSQL	0
#define NDBASETYPES	1

#define DBOW_VERSION	"0.8"

/*
 * Structure definition for columns within a table.
 */
struct	column	{
	struct	column	*next;
	int		fno;
	char		*name;
	char		*sfname;
	char		*ufname;
	char		*dfname;
	int		type;
	int		length;
	int		dprec;
	int		flags;
};

#define TYPE_TINYINT	0
#define TYPE_SMALLINT	1
#define TYPE_MEDINT	2
#define TYPE_INT	3
#define TYPE_BIGINT	4
#define TYPE_FLOAT	5
#define TYPE_DOUBLE	6
#define TYPE_NUMERIC	7
#define TYPE_DATE	8
#define TYPE_TIME	9
#define TYPE_DATETIME	10
#define TYPE_TSTAMP	11
#define TYPE_YEAR	12
#define TYPE_CHAR	13
#define TYPE_VARCHAR	14
#define TYPE_TINYBLOB	15
#define TYPE_TINYTEXT	16
#define TYPE_BLOB	17
#define TYPE_TEXT	18
#define TYPE_MEDBLOB	19
#define TYPE_MEDTEXT	20
#define TYPE_LONGBLOB	21
#define TYPE_LONGTEXT	22
#define TYPE_ENUM	23
#define TYPE_SET	24
#define NTYPES		25

#define FLAG_AUTOINC	0x0001
#define FLAG_UNSIGNED	0x0002
#define FLAG_PRIKEY	0x0004
#define FLAG_NOTNULL	0x0008
#define FLAG_UNIQUE	0x0010
#define FLAG_NATIONAL	0x0040

/*
 * Structure definition for a table.
 */
struct	table	{
	struct	table	*next;
	char		*name;
	char		*pfx;
	struct	column	*chead;
	struct	column	*ctail;
	char		*ifname;
	int		haveinsert;
	int		havedelete;
	int		havesearch;
	int		haveupdate;
	int		flags;
};

#define FLAG_DUMP	0x0001

/*
 * Structure definition for each argument to a function.
 */
struct	arg	{
	struct	arg	*next;
	struct	table	*table;
	struct	column	*column;
	char		*name;
	int		instance;
	int		type;
	int		flags;
	char		*handle;
};

#define ARGF_POINTER	0x0001

/*
 * Structure for managing set-lists.
 */
struct	set	{
	struct	set	*next;
	struct	arg	*lvalue;
	struct	arg	*rvalue;
};

/*
 * Structure for managing joins.
 */
struct	join	{
	struct	join	*next;
	struct	table	*table;
	char		*alias;
};

/*
 * Structure definition for each generated function.
 */
struct	func	{
	struct	func	*next;
	struct	table	*table;
	int		type;
	struct	column	*pkey;
	char		*name;
	int		instance;
	char		*query;
	char		*where;
	char		*order;
	int		limit;
	int		offset;
	struct	arg	*args;
	struct	join	*joins;
	char		*alias;
	struct	set	*shead;
	struct	set	*stail;
	struct	arg	*rets;
	int		flags;
};

/*
 * Structure for each code generation handler.
 */
struct	type	{
	char	*name;
	char	*m4file;
	char	*fext;
	int	cdtype;
	int	cdargs;
};

#define CDT_CODE	0
#define CDT_DBASE	1

#define CDA_NOPROC	0
#define CDA_XLATE	1

int		debug;
int		nflag;
int		mflag;
int		nerrors;
char		*prefix;
struct	type	*active;
FILE		*fofp, *hofp, *tofp;

#ifdef YYDEBUG
extern	int	yydebug;
#endif

void		doproto(char *, int);
void		docode(char *, int);
int		lexopen(char *);
void		lexclose();
struct	type	*findtype(char *);
void		linesync(char *, int, FILE *);
void		fileinc(char *, FILE *);
FILE		*m4open(char *, struct type *);
void		m4include(FILE *);
struct	table	*newtable(char *, int);
struct	table	*findtable(char *);
void		checktables();
struct	table	*findaliastable(struct func *, char *);
struct	table	*getnexttable(struct table *);
struct	column	*newcolumn(struct table *, char *, int, int, int, int);
struct	column	*findcolumn(struct table *, char *);
struct	column	*findprimarycolumn(struct table *);
void		functioncleanup();
struct	func	*newfunction(char *, int);
void		gendefaultfunc(struct table *, int, char *);
void		optimizefunc(struct func *);
void		dumpfunction(struct func *);
struct	func	*findfunction(char *);
struct	join	*newjoin(char *, char *);
struct	set	*newset(struct arg *, struct arg *);
struct	arg	*newarg(struct table *, char *);
struct	arg	*findarg(struct func *, int);
void		dumparg(struct arg *);
void		genfuncname(struct table *, char *, char *, int);
void		buildinsertsql(struct func *);
void		builddeletesql(struct func *);
void		buildsearchsql(struct func *);
void		buildupdatesql(struct func *);
void		buildwhereclause(struct func *);
void		translatesql(struct func *);
void		generatesql(struct table *, FILE *);
void		genprolog(char *, FILE *);
void		gendefs(struct table *, char *, FILE *);
void		genstr(struct table *, FILE *);
void		gencode(struct table *, FILE *);
void		genmidsect(char *, FILE *);
void		genepilog(FILE *);
int		yyparse();
void		fatal(char *);
void		yyerror(const char *);
int		gentmpf();
