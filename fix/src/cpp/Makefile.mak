# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice FIX is licensed to you under the terms described in the
# ICE_FIX_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

LIBNAME		= $(top_srcdir)\lib\icefix$(LIBSUFFIX).lib
DLLNAME		= $(top_srcdir)\bin\icefix$(ICEFIX_SOVERSION)$(LIBSUFFIX).dll

SVCLIBNAME      = $(top_srcdir)\lib\icefixservice$(LIBSUFFIX).lib
SVCDLLNAME      = $(top_srcdir)\bin\icefixservice$(ICEFIX_SOVERSION)$(LIBSUFFIX).dll

ADMIN		= $(top_srcdir)\bin\icefixadmin.exe

TARGETS         = $(LIBNAME) $(DLLNAME) $(SVCLIBNAME) $(SVCDLLNAME) $(ADMIN)

OBJS		= IceFIX.obj

SOBJS		= ServiceI.obj \
		  BridgeImpl.obj \
		  ClientImpl.obj \
		  RoutingRecordDB.obj \
		  BridgeTypes.obj \
		  ClientDB.obj \
		  MessageDB.obj \
		  MessageDBKey.obj

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

SLICE2CPPFLAGS	= -I. $(SLICE2CPPFLAGS)
CPPFLAGS	= -I. -Idummyinclude $(ICE_CPPFLAGS) $(CPPFLAGS) -DWIN32_LEAN_AND_MEAN $(QF_FLAGS)
SLINKWITH 	= $(LIBS) icefix$(LIBSUFFIX).lib icebox$(LIBSUFFIX).lib freeze$(LIBSUFFIX).lib $(QF_LIBS)
ALINKWITH 	= icegrid$(LIBSUFFIX).lib glacier2$(LIBSUFFIX).lib icefix$(LIBSUFFIX).lib $(LIBS)

!if "$(GENERATE_PDB)" == "yes"
PDBFLAGS        = /pdb:$(DLLNAME:.dll=.pdb)
APDBFLAGS       = /pdb:$(ADMIN:.exe=.pdb)
SPDBFLAGS       = /pdb:$(SVCDLLNAME:.dll=.pdb)
!endif

$(LIBNAME): $(DLLNAME)

$(DLLNAME): $(OBJS) IceFIX.res
	$(LINK) $(LD_DLLFLAGS) $(ICE_LDFLAGS) $(PDBFLAGS) $(OBJS) $(PREOUT)$@ $(PRELIBS)$(LIBS) $(RES_FILE)
	move $(DLLNAME:.dll=.lib) $(LIBNAME)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#2 && del /q $@.manifest
	@if exist $(DLLNAME:.dll=.exp) del /q $(DLLNAME:.dll=.exp)

$(SVCLIBNAME): $(SVCDLLNAME)

$(SVCDLLNAME): $(SOBJS) IceFIXService.res
	$(LINK) $(LD_DLLFLAGS) $(ICE_LDFLAGS) $(SPDBFLAGS) $(SOBJS) $(PREOUT)$@ $(PRELIBS)$(SLINKWITH) $(RES_FILE)
	move $(SVCDLLNAME:.dll=.lib) $(SVCLIBNAME)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#2 && del /q $@.manifest
	@if exist $(SVCDLLNAME:.dll=.exp) del /q $(SVCDLLNAME:.dll=.exp)

$(ADMIN): $(AOBJS) IceFIXAdmin.res 
	$(LINK) $(LD_EXEFLAGS) $(ICE_LDFLAGS) $(APDBFLAGS) $(AOBJS) $(SETARGV) $(PREOUT)$@ $(PRELIBS)$(ALINKWITH) $(ARES_FILE)
	@if exist $@.manifest echo ^ ^ ^ Embedding manifest using $(MT) && \
	    $(MT) -nologo -manifest $@.manifest -outputresource:$@;#1 && del /q $@.manifest

IceFIX.cpp $(HDIR)\IceFIX.h: $(SDIR)\IceFIX.ice
	del /q $(HDIR)\IceFIX.h IceFIX.cpp
	"$(SLICE2CPP)" --dll-export ICE_FIX_LIB_API --ice --include-dir IceFIX $(SLICE2CPPFLAGS) $(SDIR)\IceFIX.ice
	move IceFIX.h $(HDIR)

BridgeTypes.cpp BridgeTypes.h: BridgeTypes.ice
	del /q BridgeTypes.h BridgeTypes.cpp
	"$(SLICE2CPP)" $(SLICE2CPPFLAGS) BridgeTypes.ice

RoutingRecordDB.h RoutingRecordDB.cpp: BridgeTypes.ice
	del /q RoutingRecordDB.h RoutingRecordDB.cpp
	"$(SLICE2FREEZE)" $(SLICE2CPPFLAGS) --dict FIXBridge::RoutingRecordDB,string,FIXBridge::RoutingRecord \
	RoutingRecordDB BridgeTypes.ice

ClientDB.h ClientDB.cpp: BridgeTypes.ice
	del /q ClientDB.h ClientDB.cpp
	"$(SLICE2FREEZE)" $(SLICE2CPPFLAGS) --dict FIXBridge::ClientDB,string,FIXBridge::Client ClientDB \
		BridgeTypes.ice

MessageDB.h MessageDB.cpp: BridgeTypes.ice
	del /q MessageDB.h MessageDB.cpp
	"$(SLICE2FREEZE)" $(SLICE2CPPFLAGS) --dict "FIXBridge::MessageDB,long,FIXBridge::Message,sort,std::less<Ice::Long>" MessageDB BridgeTypes.ice

MessageDBKey.h MessageDBKey.cpp:
	del /q MessageDBKey.h MessageDBKey.cpp
	"$(SLICE2FREEZE)" $(SLICE2CPPFLAGS) --dict "FIXBridge::MessageDBKey,int,long"  MessageDBKey

#Scanner.cpp: Scanner.l
#	flex Scanner.l
#	del /q $@
#	echo #include "IceUtil/Config.h" > Scanner.cpp
#	type lex.yy.c >> Scanner.cpp
#	del /q lex.yy.c

#Grammar.cpp Grammar.h: Grammar.y
#	del /q Grammar.h Grammar.cpp
#	bison -dvt Grammar.y
#	move Grammar.tab.c Grammar.cpp
#	move Grammar.tab.h Grammar.h
#	del /q Grammar.output


clean::
	del /q IceFIX.cpp $(HDIR)\IceFIX.h
	del /q $(DLLNAME:.dll=.*)
	del /q $(SVCDLLNAME:.dll=.*)
	del /q $(ADMIN:.exe=.*)
	del /q IceFIXAdmin.res IceFIXMigrate.res IceFIX.res IceFIXService.res

clean::
#	del /q Grammar.cpp Grammar.h
#	del /q Scanner.cpp
	del /q ClientDB.h ClientDB.cpp
	del /q BridgeTypes.h BridgeTypes.cpp
	del /q RoutingRecordDB.h RoutingRecordDB.cpp
	del /q MessageDB.h MessageDB.cpp
	del /q MessageDBKey.h MessageDBKey.cpp

install:: all
	copy $(LIBNAME) $(install_libdir)
	copy $(DLLNAME) $(install_bindir)
	copy $(SVCLIBNAME) $(install_libdir)
	copy $(SVCDLLNAME) $(install_bindir)
	copy $(ADMIN) $(install_bindir)

!if "$(GENERATE_PDB)" == "yes"

install:: all
	copy $(DLLNAME:.dll=.pdb) $(install_bindir)
	copy $(SVCDLLNAME:.dll=.pdb) $(install_bindir)
	copy $(ADMIN:.exe=.pdb) $(install_bindir)

!endif

!include .depend.mak
