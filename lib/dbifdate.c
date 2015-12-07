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
#include <time.h>
#include <ctype.h>

#ifdef DBOW_MYSQL
#include "mysql.h"
#endif

#include "dbow.h"

/*
 *
 */
static int
numfetch(char *cp, int ndigits)
{
	int val = 0;

	while (ndigits-- && isdigit(*cp))
		val = val * 10 + *cp++ - '0';
	return(val);
}

/*
 *
 */
void
dbow_parsedate(dbow_conn *c, int *val, char *cp)
{
	int i;
	char *xp;
	unsigned long tstamp;
	struct tm tm;

	*val = 0;
	if (cp == NULL || *cp == '\0')
		return;
	/*
	 * OK, we have a live date - set up some defaults.  These
	 * are probably the wrong defaults, but if you only pull
	 * a year from the database, what do you expect me to tell
	 * you about the month or the time?
	 */
	memset((char *)&tm, 0, sizeof(struct tm));
	tm.tm_year = 1970;
	tm.tm_mon = 1;
	tm.tm_mday = 1;
	tm.tm_hour = 12;
	tm.tm_min = tm.tm_sec = 0;
	if ((xp = strpbrk(cp, ":-")) == NULL) {
		/*
		 * Date is of the form; YYYYMMDDHHMMSS.  This is a strange
		 * one because it's possible to have things like YYMMDD
		 * in here as well.  We decide this based on the number
		 * of digits and some SQL skullduggery.
		 */
		i = strlen(cp);
		tstamp = atoi(cp);
		if (i > 10) {
			/*
			 * A seconds field is present.  Strip it off.
			 */
			tm.tm_sec = tstamp % 100;
			tstamp /= 100;
		}
		if (i > 8) {
			/*
			 * Minute and hour fields are present.  Strip
			 * them off.
			 */
			tm.tm_min = tstamp % 100;
			tstamp /= 100;
			tm.tm_hour = tstamp % 100;
			tstamp /= 100;
		}
		if (i > 4) {
			/*
			 * A month-day field is present.  Strip it off.
			 */
			tm.tm_mday = tstamp % 100;
			tstamp /= 100;
		}
		if (i > 2) {
			/*
			 * A month field is present.  Strip it off.
			 */
			tm.tm_mon = tstamp % 100;
			tstamp /= 100;
		}
		/*
		 * There's always either a 2-digit or a 4-digit year.
		 */
		tm.tm_year = tstamp;
	} else {
		/*
		 * It's a DATE, TIME or DATETIME format.
		 */
		if (*xp == '-') {
			/*
			 * First, peel off the DATE portion.
			 */
			*xp++ = '\0';
			tm.tm_year = atoi(cp);
			cp = xp;
			tm.tm_mon = numfetch(cp, 2);
			cp += 2;
			/*
			 * Quick validity check - there should be a dash.
			 */
			if (*cp++ != '-')
				return;
			tm.tm_mday = numfetch(cp, 2);
			cp += 2;
			/*
			 * Strip off any whitespace.
			 */
			while (*cp == ' ')
				cp++;
			xp = strchr(cp, ':');
		}
		/*
		 * Either there's nothing left or we have a TIME field.
		 */
		if (xp != NULL) {
			*xp++ = '\0';
			tm.tm_hour = atoi(cp);
			cp = xp;
			tm.tm_min = numfetch(cp, 2);
			cp += 2;
			/*
			 * Quick validity check - there should be a colon.
			 */
			if (*cp++ != ':')
				return;
			tm.tm_sec = numfetch(cp, 2);
		}
	}
	/*
	 * Now add some sanity to the dates we've gleaned.  Note that
	 * for some ancient reason, days are (1..31) but months are
	 * (0..11).
	 */
	if (tm.tm_sec > 59)
		tm.tm_sec = 59;
	if (tm.tm_min > 59)
		tm.tm_min = 59;
	if (tm.tm_hour > 23)
		tm.tm_hour = 23;
	if (tm.tm_mday > 31 || tm.tm_mday < 1)
		tm.tm_mday = 1;
	if (tm.tm_mon > 12 || tm.tm_mon < 1)
		tm.tm_mon = 1;
	tm.tm_mon--;
	/*
	 * Years are funny things - here's the scoop; if a two-digit
	 * year has been entered (naughty!), then add the old Y2K
	 * heuristic (if < 70, then 2000->2069 otherwise 1970->1999).
	 * For 'mktime', we are interested in the number of years
	 * since the world began (on January 1st, 1900).
	 */
	if (tm.tm_year < 100) {
		if (tm.tm_year < 70)
			tm.tm_year += 2000;
		else
			tm.tm_year += 1900;
	}
	if ((tm.tm_year -= 1900) < 0)
		tm.tm_year = 0;
	/*
	 * Ask 'mktime' to do something sane with daylight-savings
	 * when it computes the actual time.
	 */
	tm.tm_isdst = -1;
	*val = mktime(&tm);
}
