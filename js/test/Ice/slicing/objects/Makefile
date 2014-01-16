# **********************************************************************
#
# Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../../..

SLICES = Test.ice

GEN_SRCS = $(patsubst 	%.ice, %.js,  $(SLICES))

SRCS = Client.js

include $(top_srcdir)/config/Make.rules

SLICE2JSFLAGS	:= $(SLICE2JSFLAGS) -I$(slicedir)

index.html: $(top_srcdir)/test/Common/index.html
	cp $(top_srcdir)/test/Common/index.html .
	
all:: index.html

clean:: 
	rm index.html