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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dbowint.h"
#include "dbow.h"

struct	table	*thead, *ttail;

/*
 *
 */
struct table *
newtable(char *name, int flags)
{
	char *cp;
	struct table *tp;
	char tmp[128];

	if ((tp = (struct table *)malloc(sizeof(struct table))) == NULL) {
		fprintf(stderr, "dbow: out of memory.\n");
		exit(1);
	}
	tp->next = NULL;
	tp->name = strdup(name);
	sprintf(tmp, "%s%s", prefix, name);
	tp->pfx = strdup(tmp);
	tp->chead = tp->ctail = NULL;
	tp->ifname = NULL;
	tp->flags = 0;
	if (thead == NULL)
		thead = tp;
	else
		ttail->next = tp;
	ttail = tp;
	for (cp = tp->name; *cp; cp++)
		if (isupper(*cp))
			*cp = tolower(*cp);
	return(tp);
}

/*
 *
 */
struct table *
findtable(char *name)
{
	struct table *tp;

	for (tp = thead; tp != NULL; tp = tp->next)
		if (strcasecmp(name, tp->name) == 0)
			return(tp);
	return(NULL);
}

/*
 *
 */
void
checktables()
{
	struct table *tp;

	for (tp = thead; tp != NULL; tp = tp->next) {
		if (!tp->haveinsert)
			gendefaultfunc(tp, DBOW_INSERT, NULL);
		if (!tp->havedelete)
			gendefaultfunc(tp, DBOW_DELETE, NULL);
		if (!tp->havesearch)
			gendefaultfunc(tp, DBOW_SEARCH, NULL);
		if (!tp->haveupdate)
			gendefaultfunc(tp, DBOW_UPDATE, NULL);
	}
}

/*
 *
 */
struct table *
getnexttable(struct table *tp)
{
	return(tp == NULL ? thead : tp->next);
}

/*
 *
 */
struct column *
newcolumn(struct table *tp, char *name, int type, int length, int dprec, int flags)
{
	struct column *cp;

	if ((cp = (struct column *)malloc(sizeof(struct column))) == NULL) {
		fprintf(stderr, "dbow: out of memory.\n");
		exit(1);
	}
	cp->next = NULL;
	cp->name = strdup(name);
	cp->sfname = cp->ufname = cp->dfname = NULL;
	cp->type = type;
	cp->length = length;
	cp->dprec = dprec;
	cp->flags = flags;
	if (tp != NULL) {
		if (tp->chead == NULL) {
			cp->fno = 0;
			tp->chead = cp;
		} else {
			cp->fno = tp->ctail->fno + 1;
			tp->ctail->next = cp;
		}
		tp->ctail = cp;
	}
	return(cp);
}

/*
 *
 */
struct column *
findcolumn(struct table *root, char *name)
{
	struct column *cp;

	for (cp = root->chead; cp != NULL; cp = cp->next)
		if (strcasecmp(cp->name, name) == 0)
			return(cp);
	return(NULL);
}

/*
 *
 */
struct column *
findprimarycolumn(struct table *root)
{
	struct column *cp;

	for (cp = root->chead; cp != NULL; cp = cp->next)
		if (cp->flags & FLAG_PRIKEY)
			return(cp);
	return(NULL);
}
