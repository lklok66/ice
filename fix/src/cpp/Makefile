# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..

LIBFILENAME     = $(call mklibfilename,IceFIX,$(ICEFIX_VERSION))
SONAME          = $(call mksoname,IceFIX,$(ICEFIX_SOVERSION))  
LIBNAME		= $(call mklibname,IceFIX)

SVCLIBFILENAME  = $(call mklibfilename,IceFIXService,$(ICEFIX_VERSION))
SVCSONAME       = $(call mksoname,IceFIXService,$(ICEFIX_SOVERSION))  
SVCLIBNAME	= $(call mklibname,IceFIXService)

ADMIN		= $(top_srcdir)/bin/icefixadmin

LIBTARGETS	= $(call mklibtargets,$(libdir)/$(LIBFILENAME),$(libdir)/$(SONAME),$(libdir)/$(LIBNAME))
SVCLIBTARGETS   = $(call mklibtargets,$(libdir)/$(SVCLIBFILENAME),$(libdir)/$(SVCSONAME),$(libdir)/$(SVCLIBNAME))

TARGETS		= $(LIBTARGETS) $(SVCLIBTARGETS) $(ADMIN)

OBJS		= IceFIX.o

SOBJS		= ServiceI.o \
		  ExecutorI.o \
		  BridgeImpl.o \
		  RoutingRecordDB.o \
		  BridgeTypes.o \
		  ClientDB.o \
		  MessageDB.o \
		  MessageDBKey.o

AOBJS		= Admin.o \
		  Grammar.o \
		  Scanner.o \
		  Parser.o \
		  IceFIX.o

SRCS		= $(OBJS:.o=.cpp) \
		  $(COBJS:.o=.cpp) \
		  $(SOBJS:.o=.cpp) \
		  $(AOBJS:.o=.cpp)

HDIR		= $(headerdir)/IceFIX
SDIR		= $(slicedir)/IceFIX

SLICE_SRCS	= $(SDIR)/IceFIX.ice BridgeTypes.ice

include $(top_srcdir)/config/Make.rules

# The -I./ is a work-around for the bug 3224.
SLICE2CPPFLAGS	:= -I./ $(SLICE2CPPFLAGS)
CPPFLAGS	:= -I. $(ICE_FLAGS) $(CPPFLAGS) $(QF_FLAGS)
LINKWITH	:= $(ICE_LIBS)
SVCLINKWITH	:= $(ICE_LIBS) -lFreeze -lIceBox -lIceFIX $(QF_LIBS)
LIBS		:= $(ICE_LIBS) -lIceFIX -lIceGrid -lGlacier2 $(QF_LIBS)

$(libdir)/$(LIBFILENAME): $(OBJS)
	rm -f $@
	$(call mkshlib,$@,$(SONAME),$(OBJS),$(LINKWITH))

$(libdir)/$(SONAME): $(libdir)/$(LIBFILENAME)
	rm -f $@
	ln -s $(LIBFILENAME) $@

$(libdir)/$(LIBNAME): $(libdir)/$(SONAME)
	rm -f $@
	ln -s $(SONAME) $@

$(libdir)/$(SVCLIBFILENAME): $(SOBJS)
	rm -f $@
	$(call mkshlib,$@,$(SVCSONAME),$(SOBJS),$(SVCLINKWITH))

$(libdir)/$(SVCSONAME): $(libdir)/$(SVCLIBFILENAME)
	rm -f $@
	ln -s $(SVCLIBFILENAME) $@

$(libdir)/$(SVCLIBNAME): $(libdir)/$(SVCSONAME)
	rm -f $@
	ln -s $(SVCSONAME) $@

$(ADMIN): $(AOBJS) $(LIBTARGETS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(AOBJS) $(READLINE_LIBS) $(LIBS)

RoutingRecordDB.h RoutingRecordDB.cpp: BridgeTypes.cpp BridgeTypes.h
	rm -f RoutingRecordDB.h RoutingRecordDB.cpp
	$(SLICE2FREEZE) $(SLICE2CPPFLAGS) --dict FIXBridge::RoutingRecordDB,string,FIXBridge::RoutingRecord \
	    RoutingRecordDB BridgeTypes.ice

ClientDB.h ClientDB.cpp: BridgeTypes.cpp BridgeTypes.h
	rm -f ClientDB.h ClientDB.cpp
	$(SLICE2FREEZE)  $(SLICE2CPPFLAGS) --dict FIXBridge::ClientDB,string,FIXBridge::Client ClientDB \
	    BridgeTypes.ice

MessageDB.h MessageDB.cpp: BridgeTypes.cpp BridgeTypes.h
	rm -f MessageDB.h MessageDB.cpp
	$(SLICE2FREEZE) $(SLICE2CPPFLAGS) --dict "FIXBridge::MessageDB,long,FIXBridge::Message,sort,std::less<Ice::Long>" MessageDB BridgeTypes.ice

MessageDBKey.h MessageDBKey.cpp: 
	rm -f MessageDBKey.h MessageDBKey.cpp
	$(SLICE2FREEZE) $(SLICE2CPPFLAGS) --dict "FIXBridge::MessageDBKey,int,long" MessageDBKey

$(HDIR)/IceFIX.h IceFIX.cpp: $(SDIR)/IceFIX.ice
	rm -f $(HDIR)/IceFIX.h IceFIX.cpp
	$(SLICE2CPP) --ice --include-dir IceFIX $(SLICE2CPPFLAGS) $<
	mv IceFIX.h $(HDIR)

clean::
	-rm -f RoutingRecordDB.h RoutingRecordDB.cpp
	-rm -f ClientDB.h ClientDB.cpp
	-rm -f MessageDB.h MessageDB.cpp
	-rm -f MessageDBKey.h MessageDBKey.cpp

install:: all
	$(call installlib,$(install_libdir),$(libdir),$(LIBFILENAME),$(SONAME),$(LIBNAME))
	$(call installlib,$(install_libdir),$(libdir),$(SVCLIBFILENAME),$(SONAME),$(LIBNAME))
	$(call installprogram,$(ADMIN),$(install_bindir))

include .depend
