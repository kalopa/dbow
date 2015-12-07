/*
 * Copyright (c) 2003, Kalopa Media Limited.
 * Copyright (c) 2015, Kalopa Research.
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
 * THIS SOFTWARE IS PROVIDED BY KALOPA RESEARCH "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA
 * RESEARCH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 */

#ifndef	_DBOW_H_
#define	_DBOW_H_

#define DBOW_INSERT	0
#define DBOW_DELETE	1
#define DBOW_SEARCH	2
#define DBOW_UPDATE	3
#define DBOW_DUMP	4
#define DBOW_OTHER	5

typedef	struct	_dbow_conn	{
	void		*dbconn;
	void		*dbres;
	int		qboff;
	int		qbsize;
	char		*qbuff;
	int		trace;
} dbow_conn;

dbow_conn	*dbow_init(char *, char *, char *, char *);
void		dbow_close(dbow_conn *);
int		dbow_query(dbow_conn *, char *, ...);
void		dbow_trace(dbow_conn *, int);
char		**dbow_fetch_row(void *);
void		dbow_free_result(void *);
int		dbow_insertid(dbow_conn *);
void		dbow_parsedate(dbow_conn *, int *, char *);
#endif /* !_DBOW_H_ */
