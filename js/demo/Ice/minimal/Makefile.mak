# **********************************************************************
#
# Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..

TARGETS = Hello.js

!if "$(OPTIMIZE)" == "yes"
TARGETS = $(TARGETS) browser\Client.min.js browser\Client.min.js.gz
!endif

!include $(top_srcdir)\config\Make.rules.mak.js

SLICE2JSFLAGS	= $(SLICE2JSFLAGS) -I"$(slicedir)"
!if "$(OPTIMIZE)" == "yes"

CLOSUREFLAGS	= $(CLOSUREFLAGS) --warning_level QUIET

browser\Client.min.js browser\Client.min.js.gz: browser\Client.js Hello.js $(libdir)\Ice.min.js
	@del /q browser\Client.min.js browser\Client.min.js.gz
	java -jar $(CLOSURE_PATH)\compiler.jar $(CLOSUREFLAGS) --js $(libdir)\Ice.js Hello.js browser\Client.js --js_output_file browser\Client.min.js
	gzip -c9 browser\Client.min.js > browser\Client.min.js.gz
!endif
