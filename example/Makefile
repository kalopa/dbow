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
.SUFFIXES:	.d .sql .m4

MYSQL_INC=/usr/local/include/mysql
MYSQL_LIB=/usr/local/lib/mysql

PREFIX?=/usr/local
DBOWDIR=$(PREFIX)/share/dbow

CFLAGS=	-O -I$(MYSQL_INC) -I../lib -DDEBUG -DYYDEBUG -DDBOW=\"$(DBOWDIR)\"

all:	sample

install: sample

clean:
	rm -f sample.c sample.h sample.o sample

sample: sample.o
	$(CC) -o sample -L../lib -L$(MYSQL_LIB) sample.o -ldbow -lmysqlclient

.d.sql:	
	../src/dbow -t mysql -o $@ $<

.d.c:
	../src/dbow -N -t C -o $@ $<

.d.m4:
	../src/dbow -m -t C -o $*.m4 $<
