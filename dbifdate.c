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
 * Revision 1.2  2003/07/28 21:48:37  dtynan
 * Minor tweaks, including fixing some gensync issues.
 *
 * Revision 1.1  2003/07/28 21:31:55  dtynan
 * First pass at an intelligent front-end for databases.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "dbow.h"

/*
 *
 */
int
dbow_fdate(dbow_row row, int pos)
{
	char *cp, datestr[12];
	struct tm tm;

	if (row[pos] == NULL || strlen(row[pos]) != 10)
		return(0);
	strcpy(datestr, row[pos]);
	datestr[4] = datestr[7] = '\0';
	tm.tm_year = atoi(datestr) - 1900;
	tm.tm_mon  = atoi(datestr + 5) - 1;
	tm.tm_mday = atoi(datestr + 8);
	return(mktime(&tm));
}

/*
 *
 */
int
dbow_idate(char *cp, int val, int len)
{
	int i = strlen(cp);
	struct tm *tmp;

	cp += i;
	len -= i;
	if (len < 11)
		return(-1);
	if (val == 0)
		strcat(cp, "SYSDATE(),");
	else {
		if ((tmp = localtime((time_t *)&val)) == NULL)
			return(-1);
		sprintf(cp, "'%04d-%02d-%02d',",
					tmp->tm_year + 1900,
					tmp->tm_mon + 1,
					tmp->tm_mday);
	}
	i += strlen(cp);
	return(i);
}
