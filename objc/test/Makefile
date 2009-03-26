# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..

include $(top_srcdir)/config/Make.rules

SUBDIRS		= Common \
		  Ice

# The tests under the iPhone only support shared libraries.
ifeq ($(COMPILE_FOR_IPHONE),yes)

  ifeq ($(STATICLIBS),yes)
    SUBDIRS	=
  endif

endif

$(EVERYTHING)::
	@for subdir in $(SUBDIRS); \
	do \
	    echo "making $@ in $$subdir"; \
	    ( cd $$subdir && $(MAKE) $@ ) || exit 1; \
	done
