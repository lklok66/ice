# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice FIX is licensed to you under the terms described in the
# ICE_FIX_LICENSE file included in this distribution.
#
# **********************************************************************

#
# Select an installation base directory. The directory will be created
# if it does not exist.
#
prefix			= C:\IceFIX-$(ICEFIX_VERSION)

#
# If Ice is not installed in a standard location, set ICE_HOME to the
# Ice installation directory.
#
#ICE_HOME               ?= C:\Program Files\ZeroC\Ice-3.4.2


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
# VC90, VC90_EXPRESS, VC100, VC100_EXPRESS
#
#CPP_COMPILER		= VC100

#
# If QuickFIX is not installed in a standard location where the
# compiler can find it, set QF_HOME to the QuickFIX installation
# directory.
#
#QF_HOME		= C:\QuickFIX


# ----------------------------------------------------------------------
# Don't change anything below this line!
# ----------------------------------------------------------------------

!if "$(CPP_COMPILER)" == ""
CPP_COMPILER		= VC100
!endif

#
# Common definitions
#
ice_language     = cpp
!if "$(USE_BIN_DIST)" == "yes" || !exist ($(top_srcdir)\..\cpp)
slice_translator = slice2cpp.exe
ice_require_cpp  = 1
!endif

!include $(top_srcdir)\config\Make.common.rules.icefix.mak

!if "$(USE_BIN_DIST)" == "yes" || !exist ($(top_srcdir)\src)

!if "$(ICEFIX_HOME)" == ""
!if "$(PROCESSOR_ARCHITECTURE)" == "AMD64" || "$(PROCESSOR_ARCHITECTUREW6432)" == "AMD64"
ICEFIX_HOME		= C:\Program Files (x86)\ZeroC\IceFIX-$(ICEFIX_VERSION)
!else
ICEFIX_HOME		= C:\Program Files\ZeroC\IceFIX-$(ICEFIX_VERSION)
!endif
!endif

!if "$(ICEFIX_HOME)" != ""
!if !exist ("$(ICEFIX_HOME)\bin\icefix$(ICEFIX_SOVERSION).dll")
!error Unable to find IceFIX-$(ICEFIX_VERSION) distribution, please verify ICEFIX_HOME is properly configured and IceFIX is correctly installed.
!endif
!endif

bindir			= $(ICEFIX_HOME)\bin
libdir			= $(ICEFIX_HOME)\lib
headerdir		= $(ICEFIX_HOME)\include

includedir		= $(ICEFIX_HOME)\include
slicedir		= $(ICEFIX_HOME)\slice
!else

bindir			= $(top_srcdir)\bin
libdir			= $(top_srcdir)\lib
headerdir		= $(top_srcdir)\include

includedir		= $(top_srcdir)\include
slicedir		= $(top_srcdir)\slice

!endif

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

!if "$(QF_HOME)" == ""
QF_HOME		= C:\QuickFIX
!endif

!if "$(QF_HOME)" != ""
QF_FLAGS	= -I"$(QF_HOME)\include"
!if "$(OPTIMIZE)" != "yes"
QF_LIBS		= "$(QF_HOME)\lib\debug\quickfix.lib" wsock32.lib
!else
QF_LIBS		= "$(QF_HOME)\lib\quickfix.lib" wsock32.lib
!endif
!endif

CPPFLAGS		= $(CPPFLAGS) -I"$(includedir)"
SLICE2CPPFLAGS		= -I"$(slicedir)" -I"$(ice_slicedir)"
LDFLAGS			= /LIBPATH:"$(libdir)" $(LDFLAGS) $(LDPLATFORMFLAGS) $(CXXFLAGS)

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

EVERYTHING		= all clean install

.SUFFIXES:
.SUFFIXES:		.ice .cpp .c .obj .res .rc

.cpp.obj::
	$(CXX) /c $(CPPFLAGS) $(CXXFLAGS) $<

.c.obj:
	$(CC) /c $(CPPFLAGS) $(CFLAGS) $<


{$(SDIR)\}.ice{$(HDIR)}.h:
	del /q $(HDIR)\$(*F).h $(*F).cpp
	"$(SLICE2CPP)" $(SLICE2CPPFLAGS) $<
	move $(*F).h $(HDIR)

.ice.cpp:
	del /q $(*F).h $(*F).cpp
	"$(SLICE2CPP)" $(SLICE2CPPFLAGS) $(*F).ice

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
