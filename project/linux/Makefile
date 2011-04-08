
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
LINUXDIR = ${SRCDIR}/linux
UNIXPROBEDIR = ${SRCDIR}/probes/unix
LINUXPROBEDIR = ${SRCDIR}/probes/linux
INDEPENDENTPROBEDIR = ${SRCDIR}/probes/independent

INCDIRS = -I/usr/local/include -I/usr/include -I$(LINUXDIR) -I${SRCDIR} -I${UNIXPROBEDIR} -I${LINUXPROBEDIR} -I${INDEPENDENTPROBEDIR}

EXECUTABLE = $(OUTDIR)/ovaldi

# General options that should be used by g++.
CPPFLAGS = -Wall -O -DLINUX $(INCDIRS)

LIBDIR = -L/usr/local/lib -L/usr/lib

# What libraries do we need?
LIBS = -lxerces-c -lxalan-c -lpcre -lpopt -lgcrypt -lldap -lblkid

# Determine what package management system is being used 
PACKAGE_RPM  = $(shell /usr/bin/env rpm  --version 2>/dev/null)
PACKAGE_DPKG = $(shell /usr/bin/env dpkg --version 2>/dev/null)

SRC_DIRS = $(SRCDIR) $(LINUXDIR) $(UNIXPROBEDIR) $(LINUXPROBEDIR) $(INDEPENDENTPROBEDIR)
CPP_FILES := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.cpp))

ifneq (${PACKAGE_RPM}, )
	LIBS += -lrpm -lrpmdb -lrpmio
	CPPFLAGS += -DPACKAGE_RPM
	CPP_FILES := $(filter-out %DPKGInfoProbe.cpp, $(CPP_FILES))
else ifneq (${PACKAGE_DPKG}, )
	LIBS += -lapt-pkg
	CPPFLAGS += -DPACKAGE_DPKG
	CPP_FILES := $(filter-out %RPMInfoProbe.cpp, $(CPP_FILES))
	CPP_FILES := $(filter-out %RPMVerifyProbe.cpp, $(CPP_FILES))
else
	CPP_FILES := $(filter-out %DPKGInfoProbe.cpp, $(CPP_FILES))
	CPP_FILES := $(filter-out %RPMInfoProbe.cpp, $(CPP_FILES))
	CPP_FILES := $(filter-out %RPMVerifyProbe.cpp, $(CPP_FILES))
endif

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

debug: CPPFLAGS = -Wall -O -static-libgcc -ggdb
debug: all
