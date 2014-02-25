# **********************************************************************
#
# Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

#
# Select an installation base directory. The directory will be created
# if it does not exist.
#
prefix                  ?= /opt/IceJS-$(ICEJS_VERSION)

#
# Define to yes for an optimized build.
#
OPTIMIZE ?= no

#
# Google Closure Compiler
#
CLOSURE_PATH=/opt/closure

#
# Closure Flags
#
CLOSUREFLAGS = --language_in ECMASCRIPT5

#
# jslint flags
#
LINTFLAGS = --verbose

# ----------------------------------------------------------------------
# Don't change anything below this line!
# ----------------------------------------------------------------------

#
# Common definitions
#
ice_language     = cpp
slice_translator = slice2js
ice_require_cpp  = 1

include $(top_srcdir)/config/Make.common.rules.js

includedir		= $(ice_dir)/include

#
# Platform specific definitions
#
include	 $(top_srcdir)/config/Make.rules.$(UNAME)

ICECPPFLAGS		= -I$(slicedir)

SLICE2CPPFLAGS		= $(ICECPPFLAGS)

ifeq ($(ice_dir), /usr) 
    LDFLAGS	= $(LDPLATFORMFLAGS) $(CXXFLAGS)
else
    CPPFLAGS	+= -I$(includedir)
    LDFLAGS	= $(LDPLATFORMFLAGS) $(CXXFLAGS) -L$(ice_dir)/$(libsubdir)$(cpp11suffix)
endif


SLICE2JS	= $(ice_js_dir)/$(binsubdir)$(cpp11suffix)/slice2js
SLICEPARSERLIB	= $(SLICE2JS)

EVERYTHING		= all clean
EVERYTHING_EXCEPT_ALL   = clean

.SUFFIXES:
.SUFFIXES:		.js

%.js: $(SDIR)/%.ice $(SLICE2JS) $(SLICEPARSERLIB)
	rm -f $(*F).js
	$(SLICE2JS) $(SLICE2JSFLAGS) $<

%.js: %.ice $(SLICE2JS) $(SLICEPARSERLIB)
	rm -f $(*F).js
	$(SLICE2JS) $(SLICE2JSFLAGS) $<

all:: $(SRCS) $(TARGETS)

clean::
	-rm -f $(TARGETS)
	-rm -f core *.o *.bak

ifneq ($(SLICE_SRCS),)
clean::
	rm -f $(addsuffix .cpp, $(basename $(notdir $(SLICE_SRCS))))
	rm -f $(addsuffix .h, $(basename $(notdir $(SLICE_SRCS))))
endif

ifneq ($(TEMPLATE_REPOSITORY),)
clean::
	rm -fr $(TEMPLATE_REPOSITORY)
endif
