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
 * Revision 1.4  2003/07/29 15:17:29  dtynan
 * Lots and lots of changes.
 *
 * Revision 1.3  2003/07/28 23:44:59  dtynan
 * Fixed a bug with the separate include file not being invoked for a split
 * generation.  Also made sure that dbow.h only got invoked once.  Fixed a
 * problem with type 'char[]' which can't be assigned.
 *
 * Revision 1.2  2003/07/28 21:48:39  dtynan
 * Minor tweaks, including fixing some gensync issues.
 *
 * Revision 1.1  2003/07/28 21:31:56  dtynan
 * First pass at an intelligent front-end for databases.
 */
#ifndef	_DBOW_H_
#define	_DBOW_H_

#include <stdio.h>

#include "mysql.h"

#define ACTION_INSERT	0
#define ACTION_DELETE	1
#define ACTION_SEARCH	2
#define ACTION_UPDATE	3
#define ACTION_DUMP	4
#define ACTION_OTHER	5

typedef MYSQL		dbow_conn;
typedef	MYSQL_ROW	dbow_row;
typedef	MYSQL_RES	dbow_res;

dbow_conn	*dbow_init();
void		dbow_close(dbow_conn *);
char		*dbow_alloc(int);
void		dbow_free(char *);
int		dbow_fint(dbow_row, int);
long		dbow_flong(dbow_row, int);
char		*dbow_fchar(dbow_row, int);
int		dbow_fdate(dbow_row, int);
float		dbow_ffloat(dbow_row, int);
double		dbow_fdouble(dbow_row, int);
char		*dbow_fdchar(dbow_row, int);
int		dbow_iint(char *, int, int);
int		dbow_ilong(char *, long, int);
int		dbow_ichar(char *, char *, int);
int		dbow_idate(char *, int, int);
int		dbow_ifloat(char *, float, int);
int		dbow_idouble(char *, double, int);
int		dbow_idchar(char *, char *, int);
int		dbow_query(dbow_conn *, char *);
int		dbow_insertid(dbow_conn *);
dbow_res	*dbow_store_result(dbow_conn *);
dbow_row	dbow_fetch_row(dbow_res *);
void		dbow_free_result(dbow_res *);

#endif /* !_DBOW_H_ */
