# **********************************************************************
#
# Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

#
# Select an installation base directory. The directory will be created
# if it does not exist.
#
prefix			= C:\Ice-$(VERSION)

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
# VC60, VC80, VC80_EXPRESS, VC90, VC90_EXPRESS, BCC2007, BCC2009
#
!if "$(CPP_COMPILER)" == ""
CPP_COMPILER		= VC80
!endif

#
# If third party libraries are not installed in the default location
# or THIRDPARTY_HOME is not set in your environment variables then
# change the following setting to reflect the installation location.
#
!if "$(CPP_COMPILER)" == "VC80_EXPRESS"
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
# For VC80 it is necessary to set the location of the manifest tool.
# This must be the 6.x version of mt.exe, not the 5.x # version!
#
!if "$(CPP_COMPILER)" == "VC80"
MT = "$(VS80COMNTOOLS)bin\mt.exe"
!else
MT = mt.exe
!endif


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

!if exist ($(top_srcdir)\..\config\Make.common.rules.mak)
!include $(top_srcdir)\..\config\Make.common.rules.mak
!else
!include $(top_srcdir)\config\Make.common.rules.mak
!endif

bindir			= $(top_srcdir)\bin
libdir			= $(top_srcdir)\lib
headerdir		= $(top_srcdir)\include

!if "$(ice_src_dist)" != ""
includedir		= $(top_srcdir)\include
!else
includedir		= $(ice_dir)\include
!endif

install_bindir		= $(prefix)\bin$(x64suffix)
install_libdir	  	= $(prefix)\lib$(x64suffix)
install_includedir	= $(prefix)\include
install_docdir		= $(prefix)\doc
install_configdir	= $(prefix)\config

SETARGV			= setargv.obj

#
# Compiler specific definitions
#
!if "$(CPP_COMPILER)" == "BCC2007" || "$(CPP_COMPILER)" == "BCC2009"
BCPLUSPLUS		= yes
!include 	$(top_srcdir)/config/Make.rules.bcc
!elseif "$(CPP_COMPILER)" == "VC60" || "$(CPP_COMPILER)" == "VC71" || \
        "$(CPP_COMPILER)" == "VC80" || "$(CPP_COMPILER)" == "VC80_EXPRESS" || \
        "$(CPP_COMPILER)" == "VC90" || "$(CPP_COMPILER)" == "VC90_EXPRESS" 
!include        $(top_srcdir)/config/Make.rules.msvc
! else
!error Invalid setting for CPP_COMPILER: $(CPP_COMPILER)
!endif

!if "$(ice_src_dist)" != ""
!if "$(THIRDPARTY_HOME)" != ""
CPPFLAGS        = -I"$(THIRDPARTY_HOME)\include" $(CPPFLAGS)
LDFLAGS         = $(PRELIBPATH)"$(THIRDPARTY_HOME)\lib$(x64suffix)" $(LDFLAGS)
!if "$(CPP_COMPILER)" == "VC60"
CPPFLAGS        = -I"$(THIRDPARTY_HOME)\include\stlport" $(CPPFLAGS)
!endif
!endif
!else
!if "$(CPP_COMPILER)" == "VC60"
CPPFLAGS        = -I"$(ice_dir)\include\stlport" $(CPPFLAGS)
!endif
!endif

!if "$(OPTIMIZE)" != "yes"
LIBSUFFIX	= $(LIBSUFFIX)d
RCFLAGS		= -D_DEBUG
!endif

OPENSSL_LIBS            = ssleay32.lib libeay32.lib
EXPAT_LIBS              = libexpat.lib

CPPFLAGS		= $(CPPFLAGS) -I$(includedir)
ICECPPFLAGS		= -I$(slicedir)
SLICE2CPPFLAGS		= $(ICECPPFLAGS)

!if "$(ice_src_dist)" != ""
LDFLAGS			= $(LDFLAGS) $(PRELIBPATH)"$(libdir)"
!else
LDFLAGS			= $(LDFLAGS) $(PRELIBPATH)"$(ice_dir)\lib$(x64suffix)"
!endif
LDFLAGS			= $(LDFLAGS) $(LDPLATFORMFLAGS) $(CXXFLAGS)

!if "$(ice_src_dist)" != ""
SLICEPARSERLIB		= $(libdir)\slice$(LIBSUFFIX).lib
SLICE2CPP		= $(bindir)\slice2cpp.exe
SLICE2XSD		= $(bindir)\slice2xsd.exe
SLICE2FREEZE		= $(bindir)\slice2freeze.exe
SLICE2DOCBOOK		= $(bindir)\slice2docbook.exe
!else
SLICEPARSERLIB		= $(ice_dir)\lib$(x64suffix)\slice$(LIBSUFFIX).lib
SLICE2CPP		= $(ice_dir)\bin$(x64suffix)\slice2cpp.exe
SLICE2XSD		= $(ice_dir)\bin$(x64suffix)\slice2xsd.exe
SLICE2FREEZE		= $(ice_dir)\bin$(x64suffix)\slice2freeze.exe
SLICE2DOCBOOK		= $(ice_dir)\bin$(x64suffix)\slice2docbook.exe
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

# Suffix set, we're using a debug build.
!if "$(LIBSUFFIX)" != ""

!if "$(LIBNAME)" != ""
clean::
	-del /q $(LIBNAME:d.lib=.lib)
	-del /q $(LIBNAME)
!endif
!if "$(DLLNAME)" != ""
clean::
	-del /q $(DLLNAME:d.dll=.*)
	-del /q $(DLLNAME:.dll=.*)
!endif

!else

!if "$(LIBNAME)" != ""
clean::
	-del /q $(LIBNAME:.lib=d.lib)
	-del /q $(LIBNAME)
!endif
!if "$(DLLNAME)" != ""
clean::
	-del /q $(DLLNAME:.dll=d.*)
	-del /q $(DLLNAME:.dll=.*)
!endif

!endif

clean::
	-del /q *.obj *.bak *.ilk *.exp *.pdb *.tds *.idb

install::
