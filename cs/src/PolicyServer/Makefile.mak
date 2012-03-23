# **********************************************************************
#
# Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
#
# This copy of Ice for Silverlight  is licensed to you under the terms
# described in the ICESL_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..

TARGETS		= $(bindir)\policyserver.exe
TARGETS_CONFIG	= $(TARGETS:.exe=.exe.config)

SRCS		= PolicyServer.cs \
		  AssemblyInfo.cs

!include $(top_srcdir)\config\Make.rules.mak.cs

MCSFLAGS	= $(MCSFLAGS) -keyfile:$(KEYFILE)
MCSFLAGS	= $(MCSFLAGS) -target:exe

$(bindir)\policyserver.exe: $(SRCS)
	$(MCS) $(MCSFLAGS) -out:$@ $(SRCS)

!if "$(DEBUG)" == "yes"
clean::
	del /q $(bindir)\policyserver.pdb
!endif

!if "$(COMPACT)" != "yes"
install::$(TARGETS)
	copy $(bindir)\policyserver.exe "$(install_bindir)"
!else
install::$(TARGETS)
	mkdir "$(install_bindir)\cf"
	copy $(bindir)\policyserver.exe "$(install_bindir)\cf"
!endif