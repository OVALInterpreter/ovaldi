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
BUILDDIR = ${OUTDIR}/obj

# source directories
SRCDIR = ../../src
LINUXDIR = ${SRCDIR}/linux
UNIXPROBEDIR = ${SRCDIR}/probes/unix
LINUXPROBEDIR = ${SRCDIR}/probes/linux
INDEPENDENTPROBEDIR = ${SRCDIR}/probes/independent
CURRENTDIR = ../project/linux

INCDIRS = -I/usr/local/include -I/usr/include -I$(LINUXDIR) -I${SRCDIR} -I${UNIXPROBEDIR} -I${LINUXPROBEDIR} -I${INDEPENDENTPROBEDIR}

EXECUTABLE = $(OUTDIR)/ovaldi

# The location of the c++ compiler.
ifeq (${CXX}, )
 CXX=/usr/bin/g++
endif


# General options that should be used by g++.
CPPFLAGS = -Wall -O

LIBDIR = -L/usr/local/lib -L/usr/lib

# What libraries do we need?
LIBS = -lxerces-c -lxalan-c -lpcre -lpopt -lgcrypt -lldap

# Determine what platform the source is being build on
PLATFORM = $(shell uname | tr '[a-z]' '[A-Z]')

# Determine what package management system is being used 
PACKAGE_RPM  = $(shell /usr/bin/env rpm  --version 2>/dev/null)
PACKAGE_DPKG = $(shell /usr/bin/env dpkg --version 2>/dev/null)
PACKAGE_MANAGERS = 

ifneq (${PACKAGE_RPM}, )
	LIBS += -lrpm -lrpmdb -lrpmio
	PACKAGE_MANAGERS += -DPACKAGE_RPM
endif
ifneq (${PACKAGE_DPKG}, )
	LIBS += -lapt-pkg
	PACKAGE_MANAGERS += -DPACKAGE_DPKG
endif

# What object code are we expecting?
OBJS = \
  Main.obj \
	AbsFileFinder.obj \
	AbsComponent.obj \
	AbsCriteria.obj \
	AbsDataCollector.obj \
	AbsEntity.obj \
	AbsEntityValue.obj \
	AbsFunctionComponent.obj \
	AbsObject.obj \
	AbsObjectCollector.obj \
	AbsProbe.obj \
	AbsState.obj \
	AbsVariable.obj \
	Analyzer.obj \
	ArithmeticFunction.obj \
	Behavior.obj \
	BeginFunction.obj \
	CollectedObject.obj \
	CollectedSet.obj \
	Common.obj \
	ComponentFactory.obj \
	ComponentValue.obj \
	ConcatFunction.obj \
	ConstantVariable.obj \
	Criteria.obj \
	Criterion.obj \
	Definition.obj \
	DocumentManager.obj \
	EndFunction.obj \
	EntityComparator.obj \
	EscapeRegexFunction.obj \
	Exception.obj \
	ExtendedDefinition.obj \
	ExternalVariable.obj \
	FileFinder.obj \
	Filter.obj \
	InterfaceProbe.obj \
	Item.obj \
	ItemEntity.obj \
	ItemFieldEntityValue.obj \
	LiteralComponent.obj \
	LocalVariable.obj \
	Log.obj \
	Object.obj \
	ObjectComponent.obj \
	ObjectEntity.obj \
	ObjectFactory.obj \
	ObjectReader.obj \
	OvalEnum.obj \
	OvalMessage.obj \
	PossibleRestrictionType.obj \
	PossibleValueType.obj \
	REGEX.obj \
	RegexCaptureFunction.obj \
	RestrictionType.obj \
	Set.obj \
	SetObject.obj \
	SplitFunction.obj \
	State.obj \
	StateEntity.obj \
	StateFieldEntityValue.obj \
	StringEntityValue.obj \
	SubstringFunction.obj \
	Test.obj \
	TestedItem.obj \
	TimeDifferenceFunction.obj \
	VariableComponent.obj \
	VariableFactory.obj \
	VariableValue.obj \
	Version.obj \
	XmlCommon.obj \
	XmlProcessor.obj \
	XslCommon.obj \
	DataCollector.obj \
	ObjectCollector.obj \
	ProbeFactory.obj \
	SystemInfo.obj \
	FileProbe.obj \
	InetListeningServersProbe.obj \
	ProcessProbe.obj \
	UnameProbe.obj \
	TextFileContentProbe.obj \
	XmlFileContentProbe.obj \
	EnvironmentVariableProbe.obj \
	FamilyProbe.obj \
	FileMd5Probe.obj \
	FileHashProbe.obj \
	VariableProbe.obj \
	RunLevelProbe.obj \
	PasswordProbe.obj \
	ShadowProbe.obj \
	TextFileContent54Probe.obj \
	InetdProbe.obj \
	XinetdProbe.obj \
	Digest.obj \
	LDAPProbe.obj

ifneq (${PACKAGE_RPM}, )
	OBJS += RPMInfoProbe.obj
endif
ifneq (${PACKAGE_DPKG}, )
	OBJS += DPKGInfoProbe.obj
endif

REL_OBJS = \
	$(BUILDDIR)/Main.obj \
	$(BUILDDIR)/AbsComponent.obj \
	$(BUILDDIR)/AbsCriteria.obj \
	$(BUILDDIR)/AbsDataCollector.obj \
	$(BUILDDIR)/AbsEntity.obj \
	$(BUILDDIR)/AbsEntityValue.obj \
	$(BUILDDIR)/AbsFileFinder.obj \
	$(BUILDDIR)/AbsFunctionComponent.obj \
	$(BUILDDIR)/AbsObject.obj \
	$(BUILDDIR)/AbsObjectCollector.obj \
	$(BUILDDIR)/AbsProbe.obj \
	$(BUILDDIR)/AbsState.obj \
	$(BUILDDIR)/AbsVariable.obj \
	$(BUILDDIR)/Analyzer.obj \
	$(BUILDDIR)/ArithmeticFunction.obj \
	$(BUILDDIR)/BeginFunction.obj \
	$(BUILDDIR)/Behavior.obj \
	$(BUILDDIR)/CollectedObject.obj \
	$(BUILDDIR)/CollectedSet.obj \
	$(BUILDDIR)/Common.obj \
	$(BUILDDIR)/ComponentFactory.obj \
	$(BUILDDIR)/ComponentValue.obj \
	$(BUILDDIR)/ConcatFunction.obj \
	$(BUILDDIR)/ConstantVariable.obj \
	$(BUILDDIR)/Criteria.obj \
	$(BUILDDIR)/Criterion.obj \
	$(BUILDDIR)/Definition.obj \
	$(BUILDDIR)/DocumentManager.obj \
	$(BUILDDIR)/EndFunction.obj \
	$(BUILDDIR)/EntityComparator.obj \
	$(BUILDDIR)/EscapeRegexFunction.obj \
	$(BUILDDIR)/Exception.obj \
	$(BUILDDIR)/ExtendedDefinition.obj \
	$(BUILDDIR)/ExternalVariable.obj \
	$(BUILDDIR)/FileFinder.obj \
	$(BUILDDIR)/Filter.obj \
	$(BUILDDIR)/InterfaceProbe.obj \
	$(BUILDDIR)/Item.obj \
	$(BUILDDIR)/ItemEntity.obj \
	$(BUILDDIR)/ItemFieldEntityValue.obj \
	$(BUILDDIR)/LiteralComponent.obj \
	$(BUILDDIR)/LocalVariable.obj \
	$(BUILDDIR)/Log.obj \
	$(BUILDDIR)/Object.obj \
	$(BUILDDIR)/ObjectComponent.obj \
	$(BUILDDIR)/ObjectEntity.obj \
	$(BUILDDIR)/ObjectFactory.obj \
	$(BUILDDIR)/ObjectReader.obj \
	$(BUILDDIR)/OvalEnum.obj \
	$(BUILDDIR)/OvalMessage.obj \
	$(BUILDDIR)/PossibleRestrictionType.obj \
	$(BUILDDIR)/PossibleValueType.obj \
	$(BUILDDIR)/REGEX.obj \
	$(BUILDDIR)/RegexCaptureFunction.obj \
	$(BUILDDIR)/RestrictionType.obj \
	$(BUILDDIR)/Set.obj \
	$(BUILDDIR)/SetObject.obj \
	$(BUILDDIR)/State.obj \
	$(BUILDDIR)/StateEntity.obj \
	$(BUILDDIR)/StateFieldEntityValue.obj \
	$(BUILDDIR)/StringEntityValue.obj \
	$(BUILDDIR)/SubstringFunction.obj \
	$(BUILDDIR)/Test.obj \
	$(BUILDDIR)/TestedItem.obj \
	$(BUILDDIR)/VariableComponent.obj \
	$(BUILDDIR)/VariableFactory.obj \
	$(BUILDDIR)/VariableValue.obj \
	$(BUILDDIR)/Version.obj \
	$(BUILDDIR)/XmlCommon.obj \
	$(BUILDDIR)/XmlProcessor.obj \
	$(BUILDDIR)/XslCommon.obj \
	$(BUILDDIR)/DataCollector.obj \
	$(BUILDDIR)/ObjectCollector.obj \
	$(BUILDDIR)/ProbeFactory.obj \
	$(BUILDDIR)/SplitFunction.obj \
	$(BUILDDIR)/SystemInfo.obj \
	$(BUILDDIR)/FileProbe.obj \
	$(BUILDDIR)/InetListeningServersProbe.obj \
	$(BUILDDIR)/ProcessProbe.obj \
	$(BUILDDIR)/UnameProbe.obj \
	$(BUILDDIR)/TextFileContentProbe.obj \
	$(BUILDDIR)/TimeDifferenceFunction.obj \
	$(BUILDDIR)/XmlFileContentProbe.obj \
	$(BUILDDIR)/EnvironmentVariableProbe.obj \
	$(BUILDDIR)/FamilyProbe.obj \
	$(BUILDDIR)/FileMd5Probe.obj \
	$(BUILDDIR)/FileHashProbe.obj \
	$(BUILDDIR)/VariableProbe.obj \
	$(BUILDDIR)/RunLevelProbe.obj \
	$(BUILDDIR)/PasswordProbe.obj \
	$(BUILDDIR)/ShadowProbe.obj \
	$(BUILDDIR)/TextFileContent54Probe.obj \
	$(BUILDDIR)/InetdProbe.obj \
	$(BUILDDIR)/XinetdProbe.obj \
	$(BUILDDIR)/Digest.obj \
	$(BUILDDIR)/LDAPProbe.obj

ifneq (${PACKAGE_RPM}, )
	REL_OBJS += $(BUILDDIR)/RPMInfoProbe.obj
endif
ifneq (${PACKAGE_DPKG}, )
	REL_OBJS += $(BUILDDIR)/DPKGInfoProbe.obj
endif

# Search the output directory for object files
vpath %.obj $(BUILDDIR)
vpath %.cpp $(SRCDIR)
vpath %.cpp $(LINUXDIR)
vpath %.cpp $(UNIXPROBEDIR)
vpath %.cpp $(LINUXPROBEDIR)
vpath %.cpp $(INDEPENDENTPROBEDIR)


# *******************************************************************
#                      Suffix Translations
# *******************************************************************

.SUFFIXES:
.SUFFIXES: .cpp .obj

.cpp.obj:
	$(CXX) -c $(CPPFLAGS) -D$(PLATFORM) $(PACKAGE_MANAGERS) $(INCDIRS) -o $(BUILDDIR)/$@ $^

# *******************************************************************
#                            Rules
# *******************************************************************

all: create-dir update set-platform


create-dir:
	@install -d ${BUILDDIR};


# OS-specific builds 
# -- these may be used later on but aren't really needed now

set-platform:
	@if [ ${PLATFORM} = SUNOS ]; then \
		make sunos; \
	fi; \
	if  [ ${PLATFORM} = LINUX ]; then \
		make linux; \
	fi;

linux: build

sunos: build


update:
#	-rm $(BUILDDIR)/Version.obj
#	cd ${SRCDIR}; ls; ./updateversion.pl; cd ${CURRENTDIR}

build : $(OBJS)
	$(CXX) $(CPPFLAGS) -D$(PLATFORM) $(REL_OBJS) $(LIBDIR) $(LIBS) -o $(EXECUTABLE)


# OS-specific cleaning
# -- These may be used later on but aren't needed right now

clean :
	@if [ $(PLATFORM) = LINUX ]; then \
		make clean-linux; \
	fi; \
	if  [ $(PLATFORM) = SUNOS ]; then \
		make clean-sunos; \
	fi;

clean-linux:
	-rm -rf $(OUTDIR)

clean-sunos:
	-rm -rf $(OUTDIR)


debug: CPPFLAGS = -Wall -O -static-libgcc -ggdb
debug: all

