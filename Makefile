#
# $Id$
#
# Copyright (c) 2003, Kalopa Media Group.  All rights reserved.
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
#
# $Log$
# Revision 1.1  2003/07/28 21:31:54  dtynan
# First pass at an intelligent front-end for databases.
#
MYSQL_INC=/usr/local/include/mysql
MYSQL_LIB=/usr/local/lib/mysql

PREFIX=	/usr/local
DBOWDIR=$(PREFIX)/share/dbow

CFLAGS=	-O -I$(MYSQL_INC) -DDEBUG -DYYDEBUG -DDBOW=\"$(DBOWDIR)\"

XSRCS=	main.c table.c type.c parse.y \
	genc.c gencpp.c genperl.c genphp.c gensql.c
XOBJS=	main.o table.o type.o parse.o \
	genc.o gencpp.o genperl.o genphp.o gensql.o
LSRCS=	dbase.c dbquery.c dbinsid.c \
	dbifchar.c dbifdate.c dbifdchar.c \
	dbifdouble.c dbiffloat.c dbifint.c dbiflong.c
LOBJS=	dbase.o dbquery.o dbinsid.o \
	dbifchar.o dbifdate.o dbifdchar.o \
	dbifdouble.o dbiffloat.o dbifint.o dbiflong.o
DBOWLIB=libdbow.a

PEFILES= c.prolog mysql.prolog perl.prolog php.prolog php.epilog

all:	dbow dbow.h $(DBOWLIB)

install: dbow dbow.h $(DBOWLIB) $(PEFILES)
	install -C -m 555 dbow $(PREFIX)/bin
	install -C -m 444 $(PEFILES) $(DBOWDIR)
	install -C -m 444 dbow.h $(PREFIX)/include
	install -C -m 444 $(DBOWLIB) $(PREFIX)/lib

clean:
	rm -f dbow $(DBOWLIB) $(XOBJS) $(LOBJS) a.out errs core
	rm -f sample.c sample.o sample

tags:	$(XSRCS) $(LSRCS)
	ctags $(XSRCS) $(LSRCS)

dbow:	$(XOBJS)
	$(CC) -o dbow $(XOBJS)

$(DBOWLIB): $(LOBJS)
	$(AR) ru $(DBOWLIB) $?

$(LOBJS): dbow.h

$(XOBJS): dbowint.h

sample: sample.o
	$(CC) -o sample -L. -L$(MYSQL_LIB) sample.o -ldbow -lmysqlclient

sample.o: sample.c
	$(CC) -O -I$(MYSQL_INC) -c sample.c

sample.c: sample.d dbow
	./dbow -t c sample.d
