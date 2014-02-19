# **********************************************************************
#
# Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

TARGETS = Chat.js ChatSession.js

!if "$(OPTIMIZE)" == "yes"
TARGETS = $(TARGETS) Client.min.js Client.min.js.gz
!endif

!include $(top_srcdir)\config\Make.rules.mak

SLICE2JSFLAGS	= $(SLICE2JSFLAGS) -I"$(slicedir)" -I.

!if "$(OPTIMIZE)" == "yes"

CLOSUREFLAGS	= $(CLOSUREFLAGS) --warning_level QUIET

Client.min.js Client.min.js.gz: Client.js Chat.js ChatSession.js $(libdir)\Ice.min.js $(libdir)\Glacier2.min.js
	@del /q Client.min.js Client.min.js.gz
	java -jar $(CLOSURE_PATH)\compiler.jar $(CLOSUREFLAGS) --js $(libdir)\Ice.js $(libdir)\Glacier2.js \
	Chat.js ChatSession.js Client.js --js_output_file Client.min.js
	gzip -c9 Client.min.js > Client.min.js.gz
!endif