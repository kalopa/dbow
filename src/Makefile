#
# Copyright (c) 2003, Kalopa Media Limited.
# Copyright (c) 2015-22, Kalopa Robotics Limited.
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
# THIS SOFTWARE IS PROVIDED BY KALOPA ROBOTICS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA ROBOTICS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
# OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
.SUFFIXES:	.d .sql

MYSQL_INC=/usr/local/include/mysql
MYSQL_LIB=/usr/local/lib/mysql

PREFIX?=/usr/local
DBOWDIR=$(PREFIX)/share/dbow

CFLAGS=	-g -Wall -I$(MYSQL_INC) -I../lib -DDEBUG -DYYDEBUG -DDBOW=\"$(DBOWDIR)\" -DDBOW_MYSQL

SRCS=	main.c table.c function.c type.c parse.y gencode.c gensql.c
OBJS=	main.o table.o function.o type.o parse.o gencode.o gensql.o
EXES=	dbow

all:	$(EXES)

install: $(EXES)
	install -d $(DBOWDIR)
	install -C -m 555 $(EXES) $(PREFIX)/bin

clean:
	rm -f $(EXES) $(OBJS) dbowgen.o y.tab.h a.out errs core

tags:	$(SRCS)
	ctags $(SRCS)

dbow:	$(OBJS)
	$(CC) -o dbow $(OBJS)

dbowgen: dbowgen.o
	$(CC) -o dbowgen dbowgen.o -L../lib -L$(MYSQL_LIB) -ldbow -lmysqlclient

$(OBJS): dbowint.h
