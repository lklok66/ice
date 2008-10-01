# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICETOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..

include $(top_srcdir)/config/Make.rules

ifeq ($(COMPILE_FOR_IPHONE),yes)
TRANSPORTS_SUBDIRS = Transports
endif

SUBDIRS		= IceUtilCpp \
		  IceCpp \
		  $(TRANSPORTS_SUBDIRS) \
		  Ice \
		  Glacier2

$(EVERYTHING)::
	@for subdir in $(SUBDIRS); \
	do \
	    if test -d $$subdir ; \
	    then \
		echo "making $@ in $$subdir"; \
		( cd $$subdir && $(MAKE) $@ ) || exit 1; \
	    fi; \
	done
