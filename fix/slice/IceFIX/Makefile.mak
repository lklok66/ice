# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice FIX is licensed to you under the terms described in the
# ICE_FIX_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

!include $(top_srcdir)/config/Make.rules.mak

install::
	@if not exist $(prefix)\slice\IceFIX \
	    @echo "Creating $(prefix)\slice\IceFIX..." && \
	    mkdir $(prefix)\slice\IceFIX

	@for %i in ( *.ice ) do \
	    @echo Installing %i && \
	    copy %i $(prefix)\slice\IceFIX
