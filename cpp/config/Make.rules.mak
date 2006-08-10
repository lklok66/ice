# **********************************************************************
#
# Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
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
# Borland C++Builder 2006 home directory. Change if different from default.
#
BCB			= C:\Program Files\Borland\BDS\4.0	

#
# If third party libraries are not installed in the default location
# change the following setting to reflect the installation location.
#
THIRDPARTY_HOME	= C:\Ice-$(VERSION)-ThirdParty

# ----------------------------------------------------------------------
# Don't change anything below this line!
# ----------------------------------------------------------------------

SHELL			= /bin/sh
VERSION			= 3.1.0
SOVERSION		= 31
bindir			= $(top_srcdir)\bin
libdir			= $(top_srcdir)\lib
includedir		= $(top_srcdir)\include
slicedir		= $(top_srcdir)\slice

install_bindir		= $(prefix)\bin

install_includedir	= $(prefix)\include
install_slicedir	= $(prefix)\slice
install_schemadir	= $(prefix)\schema
install_docdir		= $(prefix)\doc

INSTALL			= copy
INSTALL_PROGRAM		= $(INSTALL)
INSTALL_LIBRARY		= $(INSTALL)
INSTALL_DATA		= $(INSTALL)

#
# Compiler specific definitions
#
# TODO: Will need to change if/when VC++ command line builds are supported
#
!include 	$(top_srcdir)/config/Make.rules.bcc

install_libdir	  = $(prefix)\lib
libsubdir	  = lib

!if "$(OPTIMIZE)" != "yes"
LIBSUFFIX	= $(LIBSUFFIX)d
!endif

OPENSSL_LIBS            = ssleay32.lib libeay32.lib
BZIP2_LIBS              = libbz2$(LIBSUFFIX).lib
DB_LIBS                 = libdb44.lib
EXPAT_LIBS              = libexpat.lib

CPPFLAGS		= $(CPPFLAGS) -I$(includedir)
ICECPPFLAGS		= -I$(slicedir)
SLICE2CPPFLAGS		= $(ICECPPFLAGS)

LDFLAGS			= $(LDFLAGS) $(LDPLATFORMFLAGS) $(CXXFLAGS)

SLICEPARSERLIB		= $(top_srcdir)\lib\slice$(LIBSUFFIX).lib
SLICE2CPP		= $(bindir)\slice2cpp.exe
SLICE2XSD		= $(bindir)\slice2xsd.exe
SLICE2FREEZE		= $(bindir)\slice2freeze.exe
SLICE2DOCBOOK		= $(bindir)\slice2docbook.exe

EVERYTHING		= all clean install

.SUFFIXES:
.SUFFIXES:		.cpp .c .o

.cpp.o:
	$(CXX) /c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

.c.o:
	$(CC) /c $(CPPFLAGS) $(CFLAGS) -o $@ $<

all:: $(SRCS) $(TARGETS)

!if "$(TARGETS)" != ""

clean::
	-del /q $(TARGETS)

!endif

clean::
	-del /q core *.o *.bak

install::
