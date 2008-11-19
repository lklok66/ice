# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

LIBNAME		= $(top_srcdir)\lib\icestorm$(LIBSUFFIX).lib
DLLNAME		= $(top_srcdir)\bin\icestorm$(SOVERSION)$(LIBSUFFIX).dll

SVCLIBNAME_D	= $(top_srcdir)\lib\icestormserviced.lib
SVCDLLNAME_D	= $(top_srcdir)\bin\icestormservice$(SOVERSION)d.dll

SVCLIBNAME_R	= $(top_srcdir)\lib\icestormservice.lib
SVCDLLNAME_R	= $(top_srcdir)\bin\icestormservice$(SOVERSION).dll

SVCLIBNAME	= $(top_srcdir)\lib\icestormservice$(LIBSUFFIX).lib
SVCDLLNAME	= $(top_srcdir)\bin\icestormservice$(SOVERSION)$(LIBSUFFIX).dll

ADMIN		= $(top_srcdir)\bin\icestormadmin.exe
MIGRATE		= $(top_srcdir)\bin\icestormmigrate.exe

TARGETS         = $(LIBNAME) $(DLLNAME) $(SVCLIBNAME) $(SVCDLLNAME) $(ADMIN) $(MIGRATE)

OBJS		= IceStorm.obj

SERVICE_OBJS	= NodeI.obj \
		  Observers.obj \
		  LLUMap.obj \
		  Election.obj \
		  Instance.obj \
		  TraceLevels.obj \
		  Subscriber.obj \
		  TopicI.obj \
		  TopicManagerI.obj \
		  TransientTopicI.obj \
		  TransientTopicManagerI.obj \
                  SubscriberMap.obj \
		  SubscriberRecord.obj \
		  IceStormInternal.obj \
		  Service.obj

AOBJS		= Admin.obj \
		  Grammar.obj \
		  Scanner.obj \
		  Parser.obj \
		  Election.obj \
		  SubscriberRecord.obj \
		  IceStormInternal.obj

MOBJS		= Migrate.obj \
		  SubscriberRecord.obj \
                  SubscriberMap.obj \
                  LLUMap.obj \
		  LinkRecord.obj \
		  IceStormInternal.obj \
		  Election.obj \
                  V32FormatDB.obj \
                  V31FormatDB.obj \
                  V32Format.obj \
                  V31Format.obj

SRCS		= $(OBJS:.obj=.cpp) \
		  $(SOBJS:.obj=.cpp) \
		  $(AOBJS:.obj=.cpp) \
		  $(MOBJS:.obj=.cpp)

HDIR		= $(headerdir)\IceStorm
SDIR		= $(slicedir)\IceStorm

!include $(top_srcdir)\config\Make.rules.mak

CPPFLAGS	= -I.. -Idummyinclude $(CPPFLAGS) -DWIN32_LEAN_AND_MEAN
ICECPPFLAGS	= $(ICECPPFLAGS) -I..
SLICE2CPPFLAGS	= --ice --include-dir IceStorm $(SLICE2CPPFLAGS)
LINKWITH 	= $(LIBS) icestorm$(LIBSUFFIX).lib icegrid$(LIBSUFFIX).lib freeze$(LIBSUFFIX).lib icebox$(LIBSUFFIX).lib
ALINKWITH 	= $(LIBS) icestorm$(LIBSUFFIX).lib
MLINKWITH 	= $(LIBS) icestorm$(LIBSUFFIX).lib freeze$(LIBSUFFIX).lib

SLICE2FREEZECMD = $(SLICE2FREEZE) --ice --include-dir IceStorm -I.. -I$(slicedir)

!if "$(GENERATE_PDB)" == "yes"
PDBFLAGS        = /pdb:$(DLLNAME:.dll=.pdb)
SPDBFLAGS       = /pdb:$(SVCDLLNAME:.dll=.pdb)
APDBFLAGS       = /pdb:$(ADMIN:.exe=.pdb)
MPDBFLAGS       = /pdb:$(MIGRATE:.exe=.pdb)
!endif

!if "$(CPP_COMPILER)" == "BCC2007"
RES_FILE        = ,, IceStorm.res
SRES_FILE       = ,, IceStormService.res
ARES_FILE       = ,, IceStormAdmin.res
MRES_FILE       = ,, IceStormMigrate.res
!else
RES_FILE        = IceStorm.res
SRES_FILE       = IceStormService.res
ARES_FILE       = IceStormAdmin.res
MRES_FILE       = IceStormMigrate.res
!endif

$(LIBNAME): $(DLLNAME)

$(DLLNAME): $(OBJS) IceStorm.res
	$(LINK) $(LD_DLLFLAGS) $(PDBFLAGS) $(OBJS) $(PREOUT)$@ $(PRELIBS)$(LIBS) $(RES_FILE)
	move $(DLLNAME:.dll=.lib) $(LIBNAME)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#2 && del /q $@.manifest
	@if exist $(DLLNAME:.dll=.exp) del /q $(DLLNAME:.dll=.exp)

$(SVCLIBNAME): $(SVCDLLNAME)

$(SVCDLLNAME): $(SERVICE_OBJS) IceStormService.res
	$(LINK) $(LD_DLLFLAGS) $(SPDBFLAGS) $(SERVICE_OBJS) $(PREOUT)$@ $(PRELIBS)$(LINKWITH) $(SRES_FILE)
	move $(SVCDLLNAME:.dll=.lib) $(SVCLIBNAME)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#2 && del /q $@.manifest
	@if exist $(SVCDLLNAME:.dll=.exp) del /q $(SVCDLLNAME:.dll=.exp)

$(ADMIN): $(AOBJS) IceStormAdmin.res 
	$(LINK) $(LD_EXEFLAGS) $(APDBFLAGS) $(AOBJS) $(SETARGV) $(PREOUT)$@ $(PRELIBS)$(ALINKWITH) $(ARES_FILE)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

$(MIGRATE): $(MOBJS) IceStormMigrate.res
	$(LINK) $(LD_EXEFLAGS) $(MPDBFLAGS) $(MOBJS) $(SETARGV) $(PREOUT)$@ $(PRELIBS)$(MLINKWITH) $(MRES_FILE)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

LLUMap.h LLUMap.cpp: ..\IceStorm\Election.ice $(SLICE2FREEZE) $(SLICEPARSERLIB)
	del /q LLUMap.h LLUMap.cpp
	$(SLICE2FREEZECMD) --dict IceStorm::LLUMap,string,IceStormElection::LogUpdate LLUMap ..\IceStorm\Election.ice

..\IceStorm\SubscriberMap.h SubscriberMap.cpp: ..\IceStorm\SubscriberRecord.ice $(slicedir)\Ice\Identity.ice $(SLICE2FREEZE) $(SLICEPARSERLIB)
	del /q SubscriberMap.h SubscriberMap.cpp
	$(SLICE2FREEZECMD) \
	--dict IceStorm::SubscriberMap,IceStorm::SubscriberRecordKey,IceStorm::SubscriberRecord,sort \
	SubscriberMap ..\IceStorm\SubscriberRecord.ice

# Needed for migration.
V32FormatDB.h V32FormatDB.cpp: ..\IceStorm\V32Format.ice $(SLICE2FREEZE) $(SLICEPARSERLIB)
	del /q V32FormatDB.h V32FormatDB.cpp
	$(SLICE2FREEZECMD) --dict IceStorm::V32Format,Ice::Identity,IceStorm::LinkRecordSeq \
	V32FormatDB ..\IceStorm\V32Format.ice

V31FormatDB.h V31FormatDB.cpp: ..\IceStorm\V31Format.ice $(SLICE2FREEZE) $(SLICEPARSERLIB)
	del /q V31FormatDB.h V31FormatDB.cpp
	$(SLICE2FREEZECMD) --dict IceStorm::V31Format,string,IceStorm::LinkRecordDict \
	V31FormatDB ..\IceStorm\V31Format.ice

IceStorm.cpp $(HDIR)\IceStorm.h: $(SDIR)\IceStorm.ice $(SLICE2CPP) $(SLICEPARSERLIB)
	del /q $(HDIR)\IceStorm.h IceStorm.cpp
	$(SLICE2CPP) --checksum --dll-export ICE_STORM_LIB_API $(SLICE2CPPFLAGS) $(SDIR)\IceStorm.ice
	move IceStorm.h $(HDIR)

# Implicit rule to build the private IceStorm .ice files.
{..\IceStorm\}.ice{..\IceStorm\}.h:
	del /q $(*F).h $(*F).cpp
	$(SLICE2CPP) $(SLICE2CPPFLAGS) $(*F).ice

Scanner.cpp: Scanner.l
	flex Scanner.l
	del /q $@
	echo #include "IceUtil/Config.h" > Scanner.cpp
	type lex.yy.c >> Scanner.cpp
	del /q lex.yy.c

Grammar.cpp Grammar.h: Grammar.y
	del /q Grammar.h Grammar.cpp
	bison -dvt Grammar.y
	move Grammar.tab.c Grammar.cpp
	move Grammar.tab.h Grammar.h
	del /q Grammar.output

clean::
	del /q LLUMap.h LLUMap.cpp
	del /q SubscriberMap.h SubscriberMap.cpp
	del /q V32FormatDB.cpp V31FormatDB.cpp V31FormatDB.h V31FormatDB.h

clean::
	-del /q IceStorm.cpp $(HDIR)\IceStorm.h
	-del /q IceStormInternal.cpp IceStormInternal.h
	-del /q V32Migrate.cpp V32Migrate.h
	-del /q V31Migrate.cpp V31Migrate.h
	-del /q LinkRecord.cpp LinkRecord.h
	-del /q Election.cpp Election.h
	-del /q SubscriberRecord.cpp SubscriberRecord.h
	-del /q $(SVCDLLNAME_R:.dll=.*) $(SVCDLLNAME_D:.dll=.*)
	-del /q $(SVCLIBNAME_R) $(SVCLIBNAME_D)
	-del /q $(ADMIN:.exe=.*)
	-del /q $(MIGRATE:.exe=.*)
	-del /q IceStormAdmin.res IceStormMigrate.res IceStorm.res IceStormService.res

clean::
	del /q Grammar.cpp Grammar.h
	del /q Scanner.cpp

install:: all
	copy $(LIBNAME) $(install_libdir)
	copy $(DLLNAME) $(install_bindir)
	copy $(SVCLIBNAME) $(install_libdir)
	copy $(SVCDLLNAME) $(install_bindir)
	copy $(ADMIN) $(install_bindir)
	copy $(MIGRATE) $(install_bindir)


!if "$(CPP_COMPILER)" == "BCC2007" && "$(OPTIMIZE)" != "yes"

install:: all
	copy $(DLLNAME:.dll=.tds) $(install_bindir)
	copy $(SVCDLLNAME:.dll=.tds) $(install_bindir)
	copy $(ADMIN:.exe=.tds) $(install_bindir)
	copy $(MIGRATE:.exe=.tds) $(install_bindir)

!elseif "$(GENERATE_PDB)" == "yes"

install:: all
	copy $(DLLNAME:.dll=.pdb) $(install_bindir)
	copy $(SVCDLLNAME:.dll=.pdb) $(install_bindir)
	copy $(ADMIN:.exe=.pdb) $(install_bindir)
	copy $(MIGRATE:.exe=.pdb) $(install_bindir)

!endif

!include .depend
