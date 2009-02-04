# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

!if exist ($(top_srcdir)\config\Make.common.rules.mak)
!include $(top_srcdir)\config\Make.common.rules.mak
!else
!include $(top_srcdir)\..\config\Make.common.rules.mak
!endif

ICEFIX_VERSION		= 1.0.0
ICEFIX_INTVERSION	= 1.0.0
ICEFIX_SHORT_VERSION    = 1.0
ICEFIX_SOVERSION	= 10

ice_slicedir		= $(ice_dir)\slice
