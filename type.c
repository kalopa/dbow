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
 * Revision 1.1  2003/07/28 21:31:59  dtynan
 * First pass at an intelligent front-end for databases.
 */

#include <stdio.h>

#include "dbowint.h"


extern	int	line_c(char *, int, FILE *);
extern	int	str_c(struct table *, FILE *);
extern	int	code_c(struct table *, FILE *);
extern	int	line_cpp(char *, int, FILE *);
extern	int	str_cpp(struct table *, FILE *);
extern	int	code_cpp(struct table *, FILE *);
extern	int	str_perl(struct table *, FILE *);
extern	int	code_perl(struct table *, FILE *);
extern	int	str_php(struct table *, FILE *);
extern	int	code_php(struct table *, FILE *);
extern	int	str_sql(struct table *, FILE *);

struct	type	types[] = {
	{"c", "c.prolog", NULL, line_c, str_c, code_c, "c", CDT_CODE},
	{"c++", "c.prolog", NULL, line_cpp, str_cpp, code_cpp, "cpp", CDT_CODE},
	{"perl", "perl.prolog", NULL, NULL, str_perl, code_perl, "p", CDT_CODE},
	{"php", "php.prolog", "php.epilog", NULL, str_php, code_php, "php", CDT_CODE},
	{"mysql", "mysql.prolog", NULL, NULL, str_sql, NULL, "sql", CDT_DBASE},
	{NULL,NULL,NULL,NULL,NULL,0}
};

/*
 *
 */
struct type *
findtype(char *type)
{
	int i;
	struct type *tp;

	for (tp = types; tp->name != NULL; tp++)
		if (strcasecmp(type, tp->name) == 0)
			return(tp);
	return(NULL);
}
