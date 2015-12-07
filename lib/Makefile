#
# Copyright (c) 2003, Kalopa Media Limited.
# Copyright (c) 2015, Kalopa Research.
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
# THIS SOFTWARE IS PROVIDED BY KALOPA RESEARCH "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA RESEARCH BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
# OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
