# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice FIX is licensed to you under the terms described in the
# ICE_FIX_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

!include $(top_srcdir)\config\Make.rules.mak.cs

clean::
	-for %f in (db\registry\*) do if not %f == db\registry\.gitignore del /q %f
	-for %f in (distrib servers tmp) do if exist db\node\%f rmdir /s /q db\node\%f
	-for %f in (db-tp1\data\*) do if not %f == db-tp1\data\.gitignore del /q %f
	-for %f in (db-tp1\logs\*) do if not %f == db-tp1\logs\.gitignore del /q %f
	-for %f in (store\*) do if not %f == store\.gitignore del /q %f
