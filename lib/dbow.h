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
 * Revision 1.5  2004/07/05 11:20:33  dtynan
 * Added support for nested queries.
 *
 * Revision 1.4  2004/06/25 14:57:23  dtynan
 * Fixed a bug in the C template where forloops weren't working properly.
 * Added a RELEASE file, first pass at a man-page, and the basic hooks
 * for dealing with th %function block.
 *
 * Revision 1.3  2004/01/26 23:43:21  dtynan
 * Extensive changes to fix some M4 issues and some library issues.
 * Removed many of the functions which were used to parse data types
 * and made them inline instead.  Improved the M4 generator by adding
 * for loops.
 *
 * Revision 1.2  2003/11/17 13:15:19  dtynan
 * Various changes to fix errors in the back-end code.
 *
 * Revision 1.1  2003/10/14 13:00:23  dtynan
 * Major revision of the DBOW code to use M4 as a back-end instead of
 * hard-coding the output.
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
