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
#
# $Log$
# Revision 1.4  2004/09/08 11:49:47  dtynan
# New function to return the number of rows returned by a query.
#
# Revision 1.3  2004/01/26 23:43:20  dtynan
# Extensive changes to fix some M4 issues and some library issues.
# Removed many of the functions which were used to parse data types
# and made them inline instead.  Improved the M4 generator by adding
# for loops.
#
# Revision 1.2  2003/11/17 13:15:18  dtynan
# Various changes to fix errors in the back-end code.
#
# Revision 1.1  2003/10/14 13:00:20  dtynan
# Major revision of the DBOW code to use M4 as a back-end instead of
# hard-coding the output.
#
#
MYSQL_INC=/usr/local/include/mysql
MYSQL_LIB=/usr/local/lib/mysql

PREFIX?=/usr/local
DBOWDIR=$(PREFIX)/share/dbow

CFLAGS=	-Wall -I$(MYSQL_INC) -DDEBUG -DYYDEBUG -DDBOW=\"$(DBOWDIR)\" -DDBOW_MYSQL

SRCS=	dbase.c dbquery.c dbinsid.c dbhowmany.c dbifdate.c dbtrace.c
OBJS=	dbase.o dbquery.o dbinsid.o dbhowmany.o dbifdate.o dbtrace.o
DBOWLIB=libdbow.a

all:	$(DBOWLIB)

install: dbow.h $(DBOWLIB)
	install -d $(DBOWDIR)
	install -C -m 444 dbow.h $(PREFIX)/include
	install -C -m 444 $(DBOWLIB) $(PREFIX)/lib

clean:
	rm -f $(DBOWLIB) $(OBJS) a.out errs core

tags:	$(SRCS)
	ctags $(SRCS)

$(DBOWLIB): $(OBJS)
	$(AR) ru $(DBOWLIB) $?

$(OBJS): dbow.h
