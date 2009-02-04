# **********************************************************************
#
# Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

#
# Select an installation base directory. The directory will be created
# if it does not exist.
#
prefix			= C:\IceFIX-$(ICEFIX_VERSION)

#
# Define OPTIMIZE as yes if you want to build with
# optimization. Otherwise Ice is build with debug information.
#
#OPTIMIZE		= yes

#
# Define if you want pdb files to be generated for optimized/release
# builds
#
#RELEASEPDBS             = yes

#
# Specify your C++ compiler. Supported values are:
# VC60, VC80, VC80_EXPRESS, VC90, VC90_EXPRESS, BCC2007
#
!if "$(CPP_COMPILER)" == ""
CPP_COMPILER		= VC80
!endif

#
# If third party libraries are not installed in the default location
# or THIRDPARTY_HOME is not set in your environment variables then
# change the following setting to reflect the installation location.
#
!if "$(CPP_COMPILER)" == "BCC2007"
THIRDPARTY_HOME_EXT 	= BCC
!elseif "$(CPP_COMPILER)" == "VC80_EXPRESS"
THIRDPARTY_HOME_EXT	= VC80
!elseif "$(CPP_COMPILER)" == "VC90_EXPRESS"
THIRDPARTY_HOME_EXT	= VC90
!else
THIRDPARTY_HOME_EXT	= $(CPP_COMPILER)
!endif

!if "$(THIRDPARTY_HOME)" == ""
THIRDPARTY_HOME		= C:\Ice-$(VERSION)-ThirdParty-$(THIRDPARTY_HOME_EXT)
!endif

#
# For VC80 and VC80 Express it is necessary to set the location of the
# manifest tool. This must be the 6.x version of mt.exe, not the 5.x
# version!
#
# For VC80 Express mt.exe 6.x is provided by the Windows Platform SDK. 
# It is necessary to set the location of the Platform SDK through the
# PDK_HOME environment variable (see INSTALL.WINDOWS for details).
#
!if "$(CPP_COMPILER)" == "VC80"
MT = "$(VS80COMNTOOLS)bin\mt.exe"
!elseif "$(CPP_COMPILER)" == "VC80_EXPRESS"
MT = "$(PDK_HOME)\bin\mt.exe"
!else
MT = mt.exe
!endif

#
# If QuickFix is not installed in a standard location where the
# compiler can find it, set QF_HOME to the QuickFix installation
# directory.
#
#QF_HOME		?= c:\quickfix


# ----------------------------------------------------------------------
# Don't change anything below this line!
# ----------------------------------------------------------------------

#
# Common definitions
#
ice_language     = cpp
!if "$(USE_BIN_DIST)" == "yes" || !exist ($(top_srcdir)\..\cpp)
slice_translator = slice2cpp.exe
ice_require_cpp  = 1
!endif

!include $(top_srcdir)\config\Make.common.rules.icefix.mak

bindir			= $(top_srcdir)\bin
libdir			= $(top_srcdir)\lib
headerdir		= $(top_srcdir)\include

includedir		= $(top_srcdir)\include

install_bindir		= $(prefix)\bin$(x64suffix)
install_libdir	  	= $(prefix)\lib$(x64suffix)
install_includedir	= $(prefix)\include
install_docdir		= $(prefix)\doc
install_configdir	= $(prefix)\config

SETARGV			= setargv.obj

!if exist ($(top_srcdir)\config\Make.rules.msvc)
!include $(top_srcdir)\config\Make.rules.msvc
!else
!include $(top_srcdir)\..\cpp\config\Make.rules.msvc
!endif

!if "$(OPTIMIZE)" != "yes"
LIBSUFFIX	= $(LIBSUFFIX)d
RCFLAGS		= -D_DEBUG
!endif

!if "$(QF_HOME)" != ""
QF_FLAGS	= -I$(QF_HOME)\include
!if "$(OPTIMIZE)" != "yes"
QF_LIBS		= $(QF_HOME)\lib\debug\quickfix.lib wsock32.lib
!else
QF_LIBS		= $(QF_HOME)\lib\quickfix.lib wsock32.lib
!endif
!endif

slicedir		= $(top_srcdir)\slice

CPPFLAGS		= $(CPPFLAGS) -I$(includedir)
SLICE2CPPFLAGS		= -I$(slicedir) -I$(ice_slicedir)
LDFLAGS			= $(LDFLAGS) $(LDPLATFORMFLAGS) $(CXXFLAGS)

!if "$(ice_src_dist)" != ""
SLICEPARSERLIB		= $(ice_dir)\cpp\bin\slice$(LIBSUFFIX).lib
SLICE2CPP		= $(ice_dir)\cpp\bin\slice2cpp.exe
SLICE2FREEZE		= $(ice_dir)\cpp\bin\slice2freeze.exe

ICE_CPPFLAGS		= -I"$(ice_cpp_dir)\include"
!if "$(ice_cpp_dir)" == "$(ice_dir)\cpp"
ICE_LDFLAGS		= /LIBPATH:"$(ice_cpp_dir)\lib"
!else
ICE_LDFLAGS		= /LIBPATH:"$(ice_cpp_dir)\lib$(x64suffix)"
!endif
!else

SLICEPARSERLIB		= $(ice_dir)\lib$(x64suffix)\slice$(LIBSUFFIX).lib
SLICE2CPP		= $(ice_dir)\bin$(x64suffix)\slice2cpp.exe
SLICE2FREEZE		= $(ice_dir)\bin$(x64suffix)\slice2freeze.exe

ICE_CPPFLAGS		= -I"$(ice_dir)\include"
ICE_LDFLAGS		= /LIBPATH:"$(ice_dir)\lib$(x64suffix)"
!endif

ICE_LIBS		= ice$(LIBSUFFIX).lib iceutil$(LIBSUFFIX).lib

EVERYTHING		= all clean install

.SUFFIXES:
.SUFFIXES:		.ice .cpp .c .obj .res .rc

.cpp.obj::
	$(CXX) /c $(CPPFLAGS) $(CXXFLAGS) $<

.c.obj:
	$(CC) /c $(CPPFLAGS) $(CFLAGS) $<


{$(SDIR)\}.ice{$(HDIR)}.h:
	del /q $(HDIR)\$(*F).h $(*F).cpp
	$(SLICE2CPP) $(SLICE2CPPFLAGS) $<
	move $(*F).h $(HDIR)

.ice.cpp:
	del /q $(*F).h $(*F).cpp
	$(SLICE2CPP) $(SLICE2CPPFLAGS) $(*F).ice

.rc.res:
	rc $(RCFLAGS) $<


all:: $(SRCS) $(TARGETS)

!if "$(TARGETS)" != ""

clean::
	-del /q $(TARGETS)

!endif

clean::
	-del /q *.obj *.bak *.ilk *.exp *.pdb *.tds *.idb

install::
