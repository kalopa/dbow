#
# $Id$
#
# Copyright (c) 2003, Kalopa Media Limited.  All rights reserved.
#
# This is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# It is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this product; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139,
# USA.
#
# THIS SOFTWARE IS PROVIDED BY KALOPA MEDIA LIMITED "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA MEDIA LIMITED BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
# OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ABSTRACT
# This is a sample DBOW file.  The first block contains a table
# defintion, which closely resembles the SQL table code.
#
# $Log$
#

#
# Put out a C-style comment block for all file types.
#
%emit {
/*
 * $Id$
 *
 * $Log$
 */
%}

#
# Define the table.
#
%table user {
	user_id mediumint(7) NOT NULL AUTO_INCREMENT primary key,
	fname varchar(254),
	lname varchar(254),
	a1 varchar(254),
	a2 varchar(254),
	a3 varchar(254),
	city varchar(254),
	state_county varchar(254),
	postcode_zip varchar(254),
	country_id mediumint(7) NOT NULL,
	contact_phone varchar(254),
	handle varchar(254) NOT NULL,
	password varchar(254) NOT NULL
%}

#
# Define non-standard functions...
#
%search user user_id
%type C dump user

%proto

#
# An additional C function to make the resultant code executable.
#
%type C emit {

#include <stdio.h>
#include <unistd.h>

#define DB_HOST		"localhost"
#define DB_USER		"db_user"
#define DB_PWD		"db_pwd"
#define DB_NAME		"db_name"

extern	int	optind;
extern	int	opterr;
extern	char	*optarg;

void	usage();

/*
 * Test function for C
 */
main(int argc, char *argv[])
{
	int i, userid;
	char *dbhost, *dbuser, *dbpwd, *dbname;
	struct db_user *dbup;
	dbow_conn *conn;

	optind = opterr = 0;
	dbhost = DB_HOST;
	dbuser = DB_USER;
	dbpwd  = DB_PWD;
	dbname = DB_NAME;
	while ((i = getopt(argc, argv, "h:u:p:d:")) != EOF) {
		switch (i) {
		case 'h':
			dbhost = optarg;
			break;
		case 'u':
			dbuser = optarg;
			break;
		case 'p':
			dbpwd = optarg;
			break;
		case 'd':
			dbname = optarg;
			break;
		default:
			usage();
			break;
		}
	}
	if ((argc - optind) < 1)
		usage();
	if ((conn = dbow_init(dbhost, dbuser, dbpwd, dbname)) == NULL) {
		fprintf(stderr, "sample: cannot connect to database.\n");
		exit(1);
	}
	for (i = optind; i < argc; i++) {
		if ((userid = atoi(argv[i])) <= 0) {
			fprintf(stderr, "?Invalid user id: %s\n", argv[i]);
			continue;
		}
		if ((dbup = db_finduserbyuser_id(conn, userid)) == NULL) {
			fprintf(stderr, "Cannot find user ID: %d\n", userid);
			continue;
		}
		db_dumpuser(dbup, stdout);
		db_userfree(dbup);
	}
	exit(0);
}

/*
 *
 */
void
usage()
{
	fprintf(stderr, "Usage: sample [-h host][-u user][-p pwd][-d dbase] <uid>...\n");
	exit(2);
}
%}

#
# An additional PHP function...
#
%type PHP emit {
function finduniqueaddress($addr)
{
}
%}
