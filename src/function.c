/*
 * Copyright (c) 2004, Kalopa Media Limited.
 * Copyright (c) 2015-22, Kalopa Robotics Limited.
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
 * THIS SOFTWARE IS PROVIDED BY KALOPA ROBOTICS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA
 * ROBOTICS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
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
void
functioncleanup()
{
	struct func *fp;
	struct arg *ap;

	for (fp = fhead; fp != NULL; fp = fp->next) {
		/*
		 * Set the 'have' flags so we don't bother
		 * trying to auto-create basic functions.
		 */
		switch (fp->type) {
		case DBOW_INSERT:
			fp->table->haveinsert = 1;
			break;

		case DBOW_DELETE:
			fp->table->havedelete = 1;
			break;

		case DBOW_SEARCH:
			fp->table->havesearch = 1;
			break;

		case DBOW_UPDATE:
			fp->table->haveupdate = 1;
			break;
		}
	}
	/*
	 * Now check the tables and make sure we have sufficient
	 * default functions for most activities.
	 */
	checktables();
	/*
	 * Next, add pseudo-arguments for those functions which
	 * need them.
	 */
	for (fp = fhead; fp != NULL; fp = fp->next) {
		switch (fp->type) {
		case DBOW_INSERT:
			ap = newarg(fp->table, NULL);
			ap->handle = strdup("p");
			ap->flags = ARGF_POINTER;
			ap->next = fp->args;
			fp->args = ap;
			break;

		case DBOW_UPDATE:
			ap = newarg(fp->table, NULL);
			ap->handle = strdup("p");
			ap->flags = ARGF_POINTER;
			ap->next = fp->args;
			fp->args = ap;
			break;
		}
	}
	/*
	 * OK, now it's time to run through all our functions and
	 * perform any optimizations.
	 */
	for (fp = fhead; fp != NULL; fp = fp->next) {
		optimizefunc(fp);
		dumpfunction(fp);
	}
}

/*
 *
 */
void
optimizefunc(struct func *fp)
{
	int inst;
	struct arg *ap, *ap2;

	printf("Optimizing function...\n");
	/*
	 * Ok, do we have a query for this puppy?
	 */
	if (fp->query == NULL) {
		/*
		 * Punter didn't provide a query (that's a good
		 * thing, btw).  did they provide a where clause?
		 */
		if (fp->where == NULL) {
			/*
			 * No.  Let's build this up from the primary key.
			 */
			buildwhereclause(fp);
		}
		/*
		 * OK, now we're ready to make some SQL...
		 */
		switch (fp->type) {
		case DBOW_INSERT:
			buildinsertsql(fp);
			break;

		case DBOW_DELETE:
			builddeletesql(fp);
			break;

		case DBOW_SEARCH:
			buildsearchsql(fp);
			break;

		case DBOW_UPDATE:
			buildupdatesql(fp);
			break;
		}
	}
	if (active->cdargs == CDA_XLATE)
		translatesql(fp);
	/*
	 * Make sure the arguments are correct and not duplicated.
	 * Where the same argument is passed in more than once, add
	 * an 'instance' count to it.
	 */
	for (ap = fp->args; ap != NULL; ap = ap->next) {
		if (ap->instance > 0)
			continue;
		for (ap2 = ap->next, inst = 2; ap2 != NULL; ap2 = ap2->next) {
			if (strcmp(ap->name, ap2->name) == 0) {
				ap->instance = 1;
				ap2->instance = inst++;
			}
		}
	}
}

/*
 *
 */
struct func *
newfunction(char *table, int flags)
{
	struct func *fp;

	if ((fp = (struct func *)malloc(sizeof(struct func))) == NULL) {
		fprintf(stderr, "dbow: out of memory.\n");
		exit(1);
	}
	if ((fp->table = findtable(table)) == NULL) {
		yyerror("invalid table name for function");
		return(NULL);
	}
	fp->next  = NULL;
	fp->type  = DBOW_OTHER;
	fp->pkey  = NULL;
	fp->name  = NULL;
	fp->query = fp->where = fp->order = NULL;
	fp->args  = fp->rets = NULL;
	fp->joins = NULL;
	fp->alias = NULL;
	fp->shead = fp->stail = NULL;
	fp->limit = fp->offset = fp->flags = 0;
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
void
gendefaultfunc(struct table *tp, int type, char *name)
{
	struct func *fp = newfunction(tp->name, 0);

	fp->type = type;
	if (name != NULL)
		fp->name = strdup(name);
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

/*
 * Find the name of the specified table, using aliases and join aliases
 * where appropriate.
 */
struct table *
findaliastable(struct func *fp, char *tname)
{
	struct join *jp;

	if (fp->alias != NULL && strcmp(fp->alias, tname) == 0) {
		/*
		 * OK, it's an alias for this table.
		 */
		return(fp->table);
	}
	/*
	 * Now look through the list of joined tables...
	 */
	for (jp = fp->joins; jp != NULL; jp = jp->next) {
		if (strcmp(jp->alias, tname) == 0) {
			/*
			 * OK, it's an alias for a joined table.
			 */
			return(fp->table);
		}
	}
	/*
	 * OK, now look through the main list of tables...
	 */
	return(findtable(tname));
}

/*
 *
 */
struct arg *
newarg(struct table *tp, char *name)
{
	struct arg *ap;

	if ((ap = (struct arg *)malloc(sizeof(struct arg))) == NULL) {
		fprintf(stderr, "dbow: out of memory.\n");
		exit(1);
	}
	ap->next = NULL;
	ap->instance = ap->flags = 0;
	if (name != NULL) {
		if ((ap->column = findcolumn(ap->table = tp, name)) != NULL) {
			ap->name = strdup(ap->column->name);
			ap->type = ap->column->type;
		} else {
			ap->name = strdup(name);
			ap->type = 0;
		}
	} else
		ap->name = NULL;
	ap->handle = NULL;
	return(ap);
}

/*
 *
 */
struct arg *
findarg(struct func *fp, int argno)
{
	struct arg *ap = fp->args;

	while (--argno > 0)
		if (ap->next != NULL)
			ap = ap->next;
	return(ap);
}

/*
 *
 */
struct set *
newset(struct arg *lval, struct arg *rval)
{
	struct set *sp;

	if ((sp = (struct set *)malloc(sizeof(struct set))) == NULL)
		return(NULL);
	sp->next = NULL;
	sp->lvalue = lval;
	sp->rvalue = rval;
	return(sp);
}

/*
 *
 */
struct join *
newjoin(char *table, char *alias)
{
	struct join *jp;

	if ((jp = (struct join *)malloc(sizeof(struct join))) == NULL)
		return(NULL);
	jp->next = NULL;
	jp->table = findtable(table);
	jp->alias = alias;
	return(jp);
}

/*
 *
 */
void
dumparg(struct arg *ap)
{
	char *name;

	if (ap->column != NULL && ap->column->name != NULL)
		name = ap->column->name;
	else if (ap->name != NULL)
		name = ap->name;
	else
		name = ap->handle;
	if (ap->table != NULL)
		printf("Arg:\t%s.%s", ap->table->name, name);
	else
		printf("Arg:\t%s", name);
	if (ap->flags & ARGF_POINTER)
		printf("->*");
	if (ap->name != NULL) {
		if (ap->instance > 0)
			printf(" (%s%d)", ap->name, ap->instance);
		else
			printf(" (%s)", ap->name);
	}
	if (ap->handle != NULL)
		printf(" [handle:%s]", ap->handle);
	printf(" [T/F=%d/%x]\n", ap->type, ap->flags);
}

/*
 *
 */
void
dumpfunction(struct func *fp)
{
	struct join *jp;
	struct arg *ap;
	struct set *sp;

	printf(">>> Function for table: %s", fp->table->name);
	if (fp->alias != NULL)
		printf(" (%s)", fp->alias);
	printf("\nFType:\t%d\n", fp->type);
	printf("PKEY:\t%s\n", fp->pkey == NULL ? "<null>" : fp->pkey->name);
	if (fp->name != NULL) {
		if (fp->instance > 0)
			printf("Name:\t%s%d\n", fp->name, fp->instance);
		else
			printf("Name:\t%s\n", fp->name);
	}
	printf("Query:\t\"%s\"\n", fp->query == NULL ? "<null>" : fp->query);
	printf("Where:\t\"%s\"\n", fp->where == NULL ? "<null>" : fp->where);
	printf("Order:\t\"%s\"\n", fp->order == NULL ? "<null>" : fp->order);
	for (ap = fp->args; ap != NULL; ap = ap->next)
		dumparg(ap);
	for (jp = fp->joins; jp != NULL; jp = jp->next) {
		printf("Joins:\t%s", jp->table->name);
		if (jp->alias != NULL)
			printf(" (%s)", jp->alias);
		putchar('\n');
	}
	for (sp = fp->shead; sp != NULL; sp = sp->next) {
		printf("Left ");
		dumparg(sp->lvalue);
		printf("Right ");
		dumparg(sp->rvalue);
	}
	printf("Flags:\t%x\n", fp->flags);
}
