======================================================================
Introduction
======================================================================

This binary distribution contains the binaries of Ice for
Silverlight. The following components are included in this
distribution:

* The Ice for Silverlight core runtime assembly (bin\sl\Ice.dll) and
  assemblies for Ice services (bin\sl\IceGrid.dll,
  bin\sl\IcePatch2.dll, bin\sl\IceStorm.dll and bin\sl\Glacier2.dll).

* The Ice for Visual Studio Add-in plugin with Ice for Silverlight
  support (vsaddin\IceVisualStudioAddin-VS2010.dll).

* Sample programs (see below).


======================================================================
Installation
======================================================================

See the INSTALL.txt file for installation instructions.


======================================================================
Building the sample programs
======================================================================

NOTE: You must first install Ice for Silverlight before trying to open
the Visual Studio solution for the sample programs.

To build the demos:

1) Start the Visual Studio 2010 IDE and open demo\demosl.sln

2) Select Build->Build Solution to build the projects from the
   solution.


======================================================================
Overview of the sample programs
======================================================================

A brief description of each project is provided below.

* demo\Ice\sl\hello

  Demonstrates the various ways of invoking on Ice proxies using a
  simple Hello World application.

* demo\Ice\sl\bidir
* demo\Ice\compact\bidir

  Demonstrates the use of bidirectional connections. This Ice feature
  allows you to receive callbacks without requiring the server to
  establish a separate connection back to the client.

  The demo include a Silverlight client and a server designed to run
  on a Ice for .NET compact framework device.

* demo\Glacier\sl

  A very simple chat client that illustrates the Glacier2 session API.
  




