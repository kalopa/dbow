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
# Revision 1.8  2003/11/17 13:15:15  dtynan
# Various changes to fix errors in the back-end code.
#
# Revision 1.7  2003/10/14 13:15:56  dtynan
# Upgraded to new M4 format.
#
# Revision 1.6  2003/07/30 15:19:57  dtynan
# Fixed an issue with clean installs
#
# Revision 1.5  2003/07/29 15:22:48  dtynan
# Revised copyright prior to first public release.
#
# Revision 1.4  2003/07/29 15:17:29  dtynan
# Lots and lots of changes.
#
# Revision 1.3  2003/07/29 00:30:03  dtynan
# Lots of changes.
#
# Revision 1.2  2003/07/28 21:48:37  dtynan
# Minor tweaks, including fixing some gensync issues.
#
# Revision 1.1  2003/07/28 21:31:54  dtynan
# First pass at an intelligent front-end for databases.
#
DIRS=	lib src m4 doc
PREFIX?=/usr/local

all:
	@for d in $(DIRS); do $(MAKE) -C $$d all; done

install:
	@for d in $(DIRS); do $(MAKE) -C $$d install; done

clean:
	@for d in $(DIRS); do $(MAKE) -C $$d clean; done
