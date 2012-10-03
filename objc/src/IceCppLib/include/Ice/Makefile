# **********************************************************************
#
# Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../../../..

include $(top_srcdir)/config/Make.rules

INSTALL_DIR = $(CPP_SDK_DIR)/usr/local/include/Ice

all::
	@if test ! -d $(INSTALL_DIR) ; \
	then \
	    echo "Creating $(INSTALL_DIR)..." ; \
	    $(call mkdir,$(INSTALL_DIR)) ; \
	fi

	@for i in *.h ; \
	do \
	    echo "Installing $$i" ; \
	    $(INSTALL_DATA) $$i $(INSTALL_DIR)/$$i ; \
	    chmod a+r $(INSTALL_DIR)/$$i ; \
	done

depend::
