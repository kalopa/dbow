#ident "$Id$"

/*
 * $Id$
 *
 * Copyright (c) 2004, Kalopa Media Limited.  All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dbow.h"
#include "dbowint.h"

struct	func	*fhead, *ftail;

/*
 *
 */
struct func *
newfunction(char *name, int flags)
{
	struct func *fp;

	if ((fp = (struct func *)malloc(sizeof(struct func))) == NULL) {
		fprintf(stderr, "dbow: out of memory.\n");
		exit(1);
	}
	fp->next = NULL;
	fp->type = DBOW_OTHER;
	fp->name = strdup(name);
	fp->query = NULL;
	fp->ahead = fp->atail = NULL;
	fp->flags = 0;
	if (fhead == NULL)
		fhead = fp;
	else
		ftail->next = fp;
	ftail = fp;
	return(fp);
}

/*
 *
 */
struct func *
findfunction(char *name)
{
	struct func *fp;

	for (fp = fhead; fp != NULL; fp = fp->next)
		if (strcmp(name, fp->name) == 0)
			return(fp);
	return(NULL);
}

	struct	arg	*next;
	int		argno;
	struct	column	*cname;

/*
 *
 */
struct arg *
newarg(struct func *fp, int argno, struct column *cp)
{
	struct arg *ap;

	if ((ap = (struct arg *)malloc(sizeof(struct arg))) == NULL) {
		fprintf(stderr, "dbow: out of memory.\n");
		exit(1);
	}
	ap->next = NULL;
	ap->argno = argno;
	ap->cname = cp;
	if (fp != NULL) {
		if (fp->ahead == NULL)
			fp->ahead = ap;
		else
			fp->atail->next = ap;
		fp->atail = ap;
	}
	return(ap);
}

/*
 *
 */
struct arg *
findarg(struct func *fp, int argno)
{
	struct arg *ap;

	for (ap = fp->ahead; ap != NULL; ap = ap->next)
		if (ap->argno == argno)
			return(ap);
	return(NULL);
}
