#ident "$Id$"

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

#include <stdio.h>

#include "dbowint.h"

#ifndef M4DIR
#define M4DIR	"/usr/local/share/dbow"
#endif
#ifndef M4BIN
#define M4BIN	"/usr/bin/m4"
#endif

struct	type	types[] = {
	{"c", "c.m4", "c", CDT_CODE},
	{"c++", "c++.m4", "cpp", CDT_CODE},
	{"perl", "perl.m4", "p", CDT_CODE},
	{"php", "php.m4", "php", CDT_CODE},
	{"mysql", "mysql.m4", "sql", CDT_DBASE},
	{NULL,NULL,NULL,0}
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

/*
 *
 */
FILE *
m4open(char *ofname, struct type *tp)
{
	int i;
	char tmpfname[256];
	FILE *fp, *pfp;

	if (strcmp(ofname, "-") == 0)
		strcpy(tmpfname, M4BIN);
	else
		sprintf(tmpfname, "%s > %s", M4BIN, ofname);
	printf("M4: [%s]\n", tmpfname);
	if ((pfp = popen(tmpfname, "w")) == NULL) {
		fprintf(stderr, "dbow error: ");
		perror(tmpfname);
		exit(1);
	}
	sprintf(tmpfname, "%s/%s", M4DIR, tp->m4file);
	printf("M4file: [%s]\n", tmpfname);
	if ((fp = fopen(tmpfname, "r")) == NULL) {
		fprintf(stderr, "dbow: cannot find appropriate M4 template: ");
		perror(tmpfname);
		exit(1);
	}
	while ((i = fgetc(fp)) != EOF)
		fputc(i, pfp);
	fclose(fp);
	return(pfp);
}
