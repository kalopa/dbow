/*
 * Copyright (c) 2003, Kalopa Media Limited.
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
 * OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>

#include "dbow.h"

#ifdef DBOW_MYSQL
#include "mysql.h"
#if defined(__APPLE__) && defined(__MACH__)
#  define DBSOCK	"/tmp/mysql.sock"
#else
#  define DBSOCK	NULL
#endif
#endif

/*
 *
 */
dbow_conn *
dbow_init(char *host, char *user, char *pwd, char *dbase)
{
	dbow_conn *conn;

	conn = (dbow_conn *)malloc(sizeof(dbow_conn));
	if (conn == NULL)
		return(NULL);
	if ((conn->dbconn = (void *)mysql_init(NULL)) == NULL ||
				mysql_real_connect(conn->dbconn,
				host, user, pwd, dbase, 0, DBSOCK, 0) == NULL) {
		free((void *)conn);
		return(NULL);
	}
	conn->qbuff = NULL;
	conn->qboff = conn->qbsize = 0;
	return(conn);
}

/*
 *
 */
void
dbow_close(dbow_conn *conn)
{
	mysql_close(conn->dbconn);
	if (conn->qbuff != NULL)
		free(conn->qbuff);
	free((void *)conn);
}
