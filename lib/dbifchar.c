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
 * Revision 1.2  2003/10/14 14:10:56  dtynan
 * Some fixes for SQL and C, as well as 'dnl' lines in the M4 templates to
 * reduce blank lines in the output.
 *
 * Revision 1.1  2003/10/14 13:00:22  dtynan
 * Major revision of the DBOW code to use M4 as a back-end instead of
 * hard-coding the output.
 */

#include <string.h>

#include "dbow.h"

/*
 *
 */
void
dbow_fchar(char **val, dbow_row row, int pos)
{
	if (*val != NULL)
		dbow_free(*val);
	*val = (row[pos] == NULL) ? NULL : strdup(row[pos]);
}

/*
 *
 */
int
dbow_ichar(int type, char *cp, char *val, int len)
{
	int i = _dbow_iprolog(type, &cp, &len);

	if (i < 0)
		return(-1);
	if (val == NULL) {
		if (len < 4)
			return(-1);
		strcpy(cp, "NULL");
		i += 4;
	} else {
		if (type != DBOW_OTHER) {
			*cp++ = '\'';
			len--;
			i++;
		}
		while (*val != '\0' && len > 0) {
			if (*val == '\'' || *val == '\\') {
				if (len < 2)
					return(-1);
				*cp++ = '\\';
				len--;
				i++;
			}
			*cp++ = *val++;
			len--;
			i++;
		}
		if (type != DBOW_OTHER) {
			*cp++ = '\'';
			len--;
			i++;
		}
	}
	return(_dbow_iepilog(type, cp, i, len));
}

/*
 *
 */
void
dbow_fchrs(char *val, dbow_row row, int pos)
{
	if (row[pos] == NULL)
		*val = '\0';
	else
		strcpy(val, row[pos]);
}

/*
 *
 */
int
dbow_ichrs(int type, char *cp, char *val, int len)
{
	int i = _dbow_iprolog(type, &cp, &len);

	if (i < 0)
		return(-1);
	if (val == NULL) {
		if (len < 5)
			return(-1);
		strcpy(cp, "NULL,");
		i += 5;
	} else {
		*cp++ = '\'';
		len--;
		i++;
		while (*val != '\0' && len > 0) {
			if (*val == '\'' || *val == '\\') {
				if (len < 2)
					return(-1);
				*cp++ = '\\';
				len--;
				i++;
			}
			*cp++ = *val++;
			len--;
			i++;
		}
		if (len < 2)
			return(-1);
		strcpy(cp, "',");
		i += 2;
	}
	return(_dbow_iepilog(type, cp, i, len));
}

/*
 *
 */
int
dbow_strcat(char *cp, char *val, int len)
{
	int i = strlen(cp);

	cp += i;
	len -= i;
	if (val == NULL)
		return(-1);
	while (*val != '\0' && len > 0) {
		*cp++ = *val++;
		len--;
		i++;
	}
	if (len < 2)
		return(-1);
	strcpy(cp, "',");
	i += 2;
	return(i);
}
