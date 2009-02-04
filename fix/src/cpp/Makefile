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

SERVER		= $(top_srcdir)/bin/icefixserver
ADMIN		= $(top_srcdir)/bin/icefixadmin

LIBTARGETS	= $(call mklibtargets,$(libdir)/$(LIBFILENAME),$(libdir)/$(SONAME),$(libdir)/$(LIBNAME))

TARGETS		= $(LIBTARGETS) $(SERVER) $(ADMIN)

OBJS		= IceFIX.o

SOBJS		= Server.o \
		  ExecutorI.o \
		  BridgeImpl.o \
		  RoutingRecordDB.o \
		  BridgeTypes.o \
		  ClientDB.o \
		  MessageDB.o

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

BT_SLICE2CPPFLAGS := -I./ $(SLICE2CPPFLAGS)
SLICE2FREEZECMD := $(SLICE2FREEZE) -I./ $(SLICE2CPPFLAGS)
SLICE2CPPFLAGS	:= --ice --include-dir IceFIX -I. $(SLICE2CPPFLAGS)
CPPFLAGS	:= -I. $(ICE_FLAGS) $(CPPFLAGS) $(QF_FLAGS)
LINKWITH	:= $(ICE_LIBS)
LIBS 		:= $(ICE_LIBS) -lFreeze -lIceFIX $(QF_LIBS)

$(libdir)/$(LIBFILENAME): $(OBJS)
	rm -f $@
	$(call mkshlib,$@,$(SONAME),$(OBJS),$(LINKWITH))

$(libdir)/$(SONAME): $(libdir)/$(LIBFILENAME)
	rm -f $@
	ln -s $(LIBFILENAME) $@

$(libdir)/$(LIBNAME): $(libdir)/$(SONAME)
	rm -f $@
	ln -s $(SONAME) $@

$(SERVER): $(OBJS) $(SOBJS) $(LIBTARGETS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(SOBJS) $(LIBS)

$(ADMIN): $(AOBJS) $(LIBTARGETS)
	rm -f $@
	$(CXX) $(LDFLAGS) -o $@ $(AOBJS) $(READLINE_LIBS) $(LIBS)

RoutingRecordDB.h RoutingRecordDB.cpp: BridgeTypes.cpp BridgeTypes.h
	rm -f RoutingRecordDB.h RoutingRecordDB.cpp
	$(SLICE2FREEZECMD) --dict FIXBridge::RoutingRecordDB,string,FIXBridge::RoutingRecord \
	    RoutingRecordDB BridgeTypes.ice

ClientDB.h ClientDB.cpp: BridgeTypes.cpp BridgeTypes.h
	rm -f ClientDB.h ClientDB.cpp
	$(SLICE2FREEZECMD) --dict FIXBridge::ClientDB,string,FIXBridge::Client ClientDB \
	    BridgeTypes.ice

MessageDB.h MessageDB.cpp: BridgeTypes.cpp BridgeTypes.h
	rm -f MessageDB.h MessageDB.cpp
	$(SLICE2FREEZECMD) --dict FIXBridge::MessageDB,int,FIXBridge::Message MessageDB BridgeTypes.ice

BridgeTypes.h BridgeTypes.cpp: BridgeTypes.ice
	$(SLICE2CPP) $(BT_SLICE2CPPFLAGS) BridgeTypes.ice

clean::
	-rm -f RoutingRecordDB.h RoutingRecordDB.cpp
	-rm -f ClientDB.h ClientDB.cpp
	-rm -f MessageDB.h MessageDB.cpp

install:: all
	$(call installlib,$(install_libdir),$(libdir),$(LIBFILENAME),$(SONAME),$(LIBNAME))
	$(call installlib,$(install_libdir),$(libdir),$(SVCLIBFILENAME),$(SVCSONAME),$(SVCLIBNAME))
	$(call installprogram,$(SERVER),$(install_bindir))
	$(call installprogram,$(ADMIN),$(install_bindir))

include .depend
