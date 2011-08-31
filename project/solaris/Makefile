
#
#
#****************************************************************************************//
# Copyright (c) 2002-2010, The MITRE Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright notice, this list
#       of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright notice, this
#       list of conditions and the following disclaimer in the documentation and/or other
#       materials provided with the distribution.
#     * Neither the name of The MITRE Corporation nor the names of its contributors may be
#       used to endorse or promote products derived from this software without specific
#       prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
# SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#****************************************************************************************//

# output directories
OUTDIR = ./Release

# source directories
SRCDIR = ../../src
SOLARISDIR = ${SRCDIR}/solaris
UNIXDIR = ${SRCDIR}/unix
UNIXPROBEDIR = ${SRCDIR}/probes/unix
SOLARISPROBEDIR = ${SRCDIR}/probes/solaris
INDEPENDENTPROBEDIR = ${SRCDIR}/probes/independent

INCDIRS = -I/usr/local/include -I/usr/include -I$(SOLARISDIR) -I${SRCDIR} -I${UNIXPROBEDIR} -I${SOLARISPROBEDIR} -I${INDEPENDENTPROBEDIR} -I$(UNIXDIR)

EXECUTABLE = $(OUTDIR)/ovaldi

# General options that should be used by g++.
CPPFLAGS = -Wall -O -DBSD_COMP -DSUNOS $(INCDIRS)

LIBDIR = -L/usr/local/lib -L/usr/lib

# What libraries do we need?
LIBS = -lxerces-c -lxalan-c -lpcre -lgcrypt -lldap -lsocket -lnsl -llber

SRC_DIRS = $(SRCDIR) $(SOLARISDIR) $(UNIXPROBEDIR) $(SOLARISPROBEDIR) $(INDEPENDENTPROBEDIR) $(UNIXDIR)
CPP_FILES = $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.cpp))

OBJ_FILES = $(CPP_FILES:.cpp=.o)

# *******************************************************************
#                            Rules
# *******************************************************************

all: create-dir update $(EXECUTABLE)

create-dir:
	@install -d $(OUTDIR)

$(EXECUTABLE): $(OBJ_FILES)
	$(CXX) $^ $(LIBDIR) $(LIBS) -o $@

update:
#	-rm $(BUILDDIR)/Version.o
#	cd ${SRCDIR}; ls; ./updateversion.pl; cd ${CURRENTDIR}

clean :
	-rm -rf $(OUTDIR) $(OBJ_FILES)

debug: CPPFLAGS += -Wall -ggdb
debug: all
