#
# $Id$
#
# Copyright (c) 2003, Kalopa Media Group.  All rights reserved.
# Unpublished rights reserved under the copyright laws of the United
# States and/or the Republic of Ireland.
#
# The software contained herein is proprietary to and embodies the
# confidential technology of Kalopa Media Group.  Possession, use,
# duplication or dissemination of the software and media is authorized
# only pursuant to a valid written license from Kalopa Media Group.
#
# RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure by the
# U.S.  Government is subject to restrictions as set forth in
# Subparagraph (c)(1)(ii) of DFARS 252.227-7013, or in FAR 52.227-19,
# as applicable.
#
# THIS SOFTWARE IS PROVIDED BY KALOPA MEDIA GROUP "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA MEDIA GROUP BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
# OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ABSTRACT
# This table defines each club within the Sailing Club Network.  It
# contains all the necessary data for the club at the top-level.
#
#	club_id		World-unique identifier for this club
#	handle		World-unique handle for the club (ie: gbsc)
#	name		The long-form name of the club, or it's title
#	url		The URL where the club website can be found
#	a1		The first line of the clubs address
#	a2		The second line of the clubs address
#	a3		The third line of the clubs address
#	city		The city component of the clubs address
#	state_county	The state (or country) of the clubs address
#	postcode_zip	The zip (or postcode) of the clubs address
#	country_id	A link to the country table for the home country
#
# $Log$
# Revision 1.3  2003/07/29 00:30:04  dtynan
# Lots of changes.
#
# Revision 1.2  2003/07/28 21:48:40  dtynan
# Minor tweaks, including fixing some gensync issues.
#
# Revision 1.1  2003/07/28 21:31:59  dtynan
# First pass at an intelligent front-end for databases.
#
# Revision 1.4  2003/02/27 11:40:16  dtynan
# Changed data to use fictitious club so as not to stomp on the GBSC
# data, and added table comments to the SQL files to explain the field
# usage.
#
# Revision 1.3  2003/02/16 22:15:49  dtynan
# Database Column Harmonization Project, Phase I.  Standardized the name
# of the list/exe_list/club/user to 'handle'.
#
# Revision 1.2  2003/02/16 18:34:04  dtynan
# Changes to database tables based on Kierans updates and extensions.
#
# Revision 1.1  2003/02/16 13:33:18  dtynan
# First pass at creating SQL tables and data.
#

#
# Define the table.
#
%table club {
	club_id mediumint(7) NOT NULL AUTO_INCREMENT PRIMARY KEY,
	handle varchar(254) NOT NULL UNIQUE,
	name varchar(254) NOT NULL,
	url varchar(254) NOT NULL,
	a1 varchar(254),
	a2 varchar(254),
	a3 varchar(254),
	city varchar(254),
	state_county varchar(254),
	postcode_zip varchar(254),
	country_id mediumint(7) NOT NULL
%}

#
# Define the search functions
#
%insert club clubinsert
%search club club_id findbyclubid
%search club handle
%type php search club url
%type C update club club_id
%type perl delete club club_id
%type C delete club url

%dump club

%proto

#
# Include a main function to keep the linker happy.
#
%type C emit {

#include <stdio.h>

main()
{
	int i;
	struct db_club *p;
	dbow_conn *conn;

	if ((conn = dbow_init("sailclub", "Boomer", "sailclub")) == NULL) {
		fprintf(stderr, "sample: cannot connect to database.\n");
		exit(1);
	}
	for (i = 0; i < 10; i++)
		if ((p = findbyclubid(conn, i)) != NULL)
			dump_club(p, stdout);
}

%}
