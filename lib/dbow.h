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
 */
#ifndef	_DBOW_H_
#define	_DBOW_H_

#include <stdio.h>

#include "mysql.h"

#define dbow_freeint(p)
#define dbow_freelong(p)
#define dbow_freechar(p)	{if ((p) != NULL) dbow_free(p); }
#define dbow_freechrs(p)
#define dbow_freedate(p)
#define dbow_freefloat(p)
#define dbow_freedouble(p)
#define dbow_freedchar(p)	{if ((p) != NULL) dbow_free(p); }

#define dbow_dumpint(fp, p)	{fprintf(fp, #p " = %d\n", p);}
#define dbow_dumplong(fp, p)	{fprintf(fp, #p " = %d\n", p);}
#define dbow_dumpchar(fp, p)	{fprintf(fp, #p " = %s\n", p == NULL ? "(null)" : p);}
#define dbow_dumpchrs(fp, p)	{fprintf(fp, #p " = %d\n", p);}
#define dbow_dumpdate(fp, p)	{fprintf(fp, #p " = %d\n", p);}
#define dbow_dumpfloat(fp, p)	{fprintf(fp, #p " = %f\n", p);}
#define dbow_dumpdouble(fp, p)	{fprintf(fp, #p " = %lf\n", p);}
#define dbow_dumpdchar(fp, p)	{fprintf(fp, #p " = %s\n", p == NULL ? "(null)" : p);}

#define DBOW_INSERT	0
#define DBOW_DELETE	1
#define DBOW_SEARCH	2
#define DBOW_UPDATE	3
#define DBOW_DUMP	4
#define DBOW_OTHER	5

typedef MYSQL		dbow_conn;
typedef	MYSQL_ROW	dbow_row;
typedef	MYSQL_RES	dbow_res;

dbow_conn	*dbow_init(char *, char *, char *, char *);
void		dbow_close(dbow_conn *);
char		*dbow_alloc(int);
void		dbow_free(char *);
int		dbow_fint(dbow_row, int);
long		dbow_flong(dbow_row, int);
char		*dbow_fchar(dbow_row, int);
void		dbow_fchrs(char *, dbow_row, int);
int		dbow_fdate(dbow_row, int);
float		dbow_ffloat(dbow_row, int);
double		dbow_fdouble(dbow_row, int);
char		*dbow_fdchar(dbow_row, int);
int		dbow_iint(int, char *, int, int);
int		dbow_ilong(int, char *, long, int);
int		dbow_ichar(int, char *, char *, int);
int		dbow_ichrs(int, char *, char *, int);
int		dbow_idate(int, char *, int, int);
int		dbow_ifloat(int, char *, float, int);
int		dbow_idouble(int, char *, double, int);
int		dbow_idchar(int, char *, char *, int);
int		dbow_query(dbow_conn *, char *);
int		dbow_insertid(dbow_conn *);
dbow_res	*dbow_store_result(dbow_conn *);
dbow_row	dbow_fetch_row(dbow_res *);
void		dbow_free_result(dbow_res *);

#endif /* !_DBOW_H_ */
