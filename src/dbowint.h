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
 * Revision 1.2  2003/11/17 13:15:20  dtynan
 * Various changes to fix errors in the back-end code.
 *
 * Revision 1.1  2003/10/14 13:00:26  dtynan
 * Major revision of the DBOW code to use M4 as a back-end instead of
 * hard-coding the output.
 *
 */

#define DBASE_MYSQL	0
#define NDBASETYPES	1

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
	int		flags;
};

#define FLAG_DUMP	0x0001

/*
 * Structure for each code generation handler.
 */
struct	type	{
	char	*name;
	char	*m4file;
	char	*fext;
	int	cdtype;
};

#define CDT_CODE	0
#define CDT_DBASE	1

int		debug;
int		nflag;
int		mflag;
int		nerrors;
char		*prefix;
struct	type	*active;
FILE		*fofp, *hofp;

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
struct	table	*findtable(char *name);
struct	table	*getnexttable(struct table *);
struct	column	*newcolumn(struct table *, char *, int, int, int, int);
struct	column	*findcolumn(struct table *, char *);
void		genfuncname(struct table *, char *, char *, int);
void		generatesql(struct table *, FILE *);
void		gendefs(struct table *, char *, FILE *);
void		genstr(struct table *, FILE *);
void		gencode(struct table *, FILE *);
void		genepilog(FILE *);
int		yyparse();
void		yyerror(const char *);
