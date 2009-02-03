# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

LIBNAME		= $(top_srcdir)\lib\icefix$(LIBSUFFIX).lib
DLLNAME		= $(top_srcdir)\bin\icefix$(SOVERSION)$(LIBSUFFIX).dll

ADMIN		= $(top_srcdir)\bin\icefixadmin.exe
SERVER		= $(top_srcdir)\bin\icefixserver.exe

TARGETS         = $(LIBNAME) $(DLLNAME) $(ADMIN) $(SERVER)

OBJS		= IceFIX.obj

SOBJS		= Server.obj \
		  ExecutorI.obj \
		  BridgeImpl.obj \
		  RoutingRecordDB.obj \
		  BridgeTypes.obj \
		  ClientDB.obj \
		  MessageDB.obj

AOBJS		= Admin.obj \
		  Grammar.obj \
		  Scanner.obj \
		  Parser.obj

SRCS		= $(OBJS:.obj=.cpp) \
		  $(SOBJS:.obj=.cpp) \
		  $(AOBJS:.obj=.cpp)

HDIR		= $(headerdir)\IceFIX
SDIR		= $(slicedir)\IceFIX

!include $(top_srcdir)\config\Make.rules.mak

ICECPPFLAGS	= $(ICECPPFLAGS) -I..
SLICE2CPPFLAGS	= --ice --include-dir IceFIX -I.. $(SLICE2CPPFLAGS)
SLICE2FREEZECMD = $(SLICE2FREEZE) $(SLICE2CPPFLAGS)
CPPFLAGS	= -I.. -Idummyinclude $(CPPFLAGS) -DWIN32_LEAN_AND_MEAN $(QF_FLAGS)
SLINKWITH 	= $(LIBS) icefix$(LIBSUFFIX).lib freeze$(LIBSUFFIX).lib $(QF_LIBS)
ALINKWITH 	= $(LIBS) icefix$(LIBSUFFIX).lib

SLICE2FREEZECMD = $(SLICE2FREEZE) --ice --include-dir IceFIX -I.. -I$(slicedir)

!if "$(GENERATE_PDB)" == "yes"
PDBFLAGS        = /pdb:$(DLLNAME:.dll=.pdb)
APDBFLAGS       = /pdb:$(ADMIN:.exe=.pdb)
SPDBFLAGS       = /pdb:$(SERVER:.exe=.pdb)
!endif

!if "$(CPP_COMPILER)" == "BCC2007"
RES_FILE        = IceFIX.res
ARES_FILE       = IceFIXAdmin.res
MRES_FILE       = IceFIXServer.res
!endif

$(LIBNAME): $(DLLNAME)

$(DLLNAME): $(OBJS) IceFIX.res
	$(LINK) $(LD_DLLFLAGS) $(PDBFLAGS) $(OBJS) $(PREOUT)$@ $(PRELIBS)$(LIBS) $(RES_FILE)
	move $(DLLNAME:.dll=.lib) $(LIBNAME)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#2 && del /q $@.manifest
	@if exist $(DLLNAME:.dll=.exp) del /q $(DLLNAME:.dll=.exp)

$(ADMIN): $(AOBJS) IceFIXAdmin.res 
	$(LINK) $(LD_EXEFLAGS) $(APDBFLAGS) $(AOBJS) $(SETARGV) $(PREOUT)$@ $(PRELIBS)$(ALINKWITH) $(ARES_FILE)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

$(SERVER): $(SOBJS) IceFIXServer.res
	$(LINK) $(LD_EXEFLAGS) $(SPDBFLAGS) $(SOBJS) $(SETARGV) $(PREOUT)$@ $(PRELIBS)$(SLINKWITH) $(SRES_FILE)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

IceFIX.cpp $(HDIR)\IceFIX.h: $(SDIR)\IceFIX.ice $(SLICE2CPP) $(SLICEPARSERLIB)
	del /q $(HDIR)\IceFIX.h IceFIX.cpp
	$(SLICE2CPP) --dll-export ICE_FIX_LIB_API $(SLICE2CPPFLAGS) $(SDIR)\IceFIX.ice
	move IceFIX.h $(HDIR)

BridgeTypes.cpp BridgeTypes.h: ..\IceFIX\BridgeTypes.ice $(SLICE2CPP) $(SLICEPARSERLIB)
	del /q BridgeTypes.h BridgeTypes.cpp
	$(SLICE2CPP) $(SLICE2CPPFLAGS) ..\IceFIX\BridgeTypes.ice

RoutingRecordDB.h RoutingRecordDB.cpp: ..\IceFix\BridgeTypes.ice $(SLICE2FREEZE) $(SLICEPARSERLIB)
	del /q RoutingRecordDB.h RoutingRecordDB.cpp
	$(SLICE2FREEZECMD) --dict FIXBridge::RoutingRecordDB,string,FIXBridge::RoutingRecord \
	RoutingRecordDB ..\IceFix\BridgeTypes.ice

ClientDB.h ClientDB.cpp: ..\IceFix\BridgeTypes.ice $(SLICE2FREEZE) $(SLICEPARSERLIB)
	del /q ClientDB.h ClientDB.cpp
	$(SLICE2FREEZECMD) --dict FIXBridge::ClientDB,string,FIXBridge::Client ClientDB \
		..\IceFix\BridgeTypes.ice

MessageDB.h MessageDB.cpp: ..\IceFix\BridgeTypes.ice $(SLICE2FREEZE) $(SLICEPARSERLIB)
	del /q MessageDB.h MessageDB.cpp
	$(SLICE2FREEZECMD) --dict FIXBridge::MessageDB,int,FIXBridge::Message MessageDB ..\IceFix\BridgeTypes.ice

# Implicit rule to build the private IceFIX .ice files.
{..\IceFIX\}.ice{..\IceFIX\}.h:
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
	del /q IceFIX.cpp $(HDIR)\IceFIX.h
	del /q $(DLLNAME:.dll=.*)
	del /q $(ADMIN:.exe=.*)
	del /q $(SERVER:.exe=.*)
	del /q IceFIXAdmin.res IceFIXMigrate.res IceFIX.res IceFIXService.res

clean::
	del /q Grammar.cpp Grammar.h
	del /q Scanner.cpp
	del /q ClientDB.h ClientDB.cpp
	del /q BridgeTypes.h BridgeTypes.cpp
	del /q RoutingRecordDB.h RoutingRecordDB.cpp
	del /q MessageDB.h MessageDB.cpp

install:: all
	copy $(LIBNAME) $(install_libdir)
	copy $(DLLNAME) $(install_bindir)
	copy $(ADMIN) $(install_bindir)
	copy $(SERVER) $(install_bindir)


!if "$(GENERATE_PDB)" == "yes"

install:: all
	copy $(DLLNAME:.dll=.pdb) $(install_bindir)
	copy $(ADMIN:.exe=.pdb) $(install_bindir)
	copy $(SERVER:.exe=.pdb) $(install_bindir)

!endif

!include .depend
