# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice FIX is licensed to you under the terms described in the
# ICE_FIX_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

TARGETS		= client.exe
TARGETS_CONFIG	= $(TARGETS:.exe=.exe.config)

C_SRCS		= Client.cs

!include $(top_srcdir)\config\Make.rules.mak.cs

MCSFLAGS	= $(MCSFLAGS) -target:exe

client.exe: $(C_SRCS)
	$(MCS) $(MCSFLAGS) -out:$@ -r:"$(ice_refdir)\Ice.dll" -r:"$(ice_refdir)\IceGrid.dll" -r:"$(ice_refdir)\Glacier2.dll" -r:"$(refdir)\IceFIX.dll"  $(QF_FLAGS) $(C_SRCS)

clean::
	-for %f in (db\registry\*) do if not %f == db\registry\.gitignore del /q %f
	-for %f in (distrib servers tmp) do if exist db\node\%f rmdir /s /q db\node\%f
	-for %f in (db-tp1\*) do if not %f == db-tp1\.gitignore del /q %f
	-for %f in (store-tp1\*) do if not %f == store-tp1\.gitignore del /q %f
	-for %f in (db-tp2\*) do if not %f == db-tp2\.gitignore del /q %f
	-for %f in (store-tp2\*) do if not %f == store-tp2\.gitignore del /q %f

!include .depend
