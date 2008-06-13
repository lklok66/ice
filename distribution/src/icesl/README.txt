======================================================================
About this distribution
======================================================================

This distribution is a binary release of Ice for Silverlight 0.2.0
including the following components:

- The Ice for Silverlight runtime assembly, as well as the Ice for 
  Silverlight bridge and Slice files.

- The Slice-to-Silverlight translator.

- The Ice for .NET runtime assembly.


======================================================================
Requirements
======================================================================


Windows Version
---------------

Ice for Silverlight has been extensively tested on:

- Windows XP x86 and x64
- Windows Vista x86 and x64


Browser
-------

Ice for Silverlight has been tested with:

- Internet Explorer 7.0 
- Firefox 3.0


Compiler
--------

Ice for Silverlight requires:

- Microsoft Visual Studio 2008

  http://msdn2.microsoft.com/en-us/vstudio/aa700831.aspx

- Microsoft Visual Studio 2008 Silverlight Tools

  http://www.microsoft.com/downloads/details.aspx?FamilyID=50A9EC01-267B-4521-B7D7-C0DBA8866434&displaylang=en


Silverlight Runtime
-------------------

- Silverlight 2.0 Beta 2

  Downloaded as part of the Visual Studio Tools above.


======================================================================
Package Contents
======================================================================


Ice for Silverlight
-------------------

There are two main components included in the package. The first
component is the Ice for Silverlight assembly (IceSL.dll), which all
Ice for Silverlight applications must reference.

The second component is the Ice for Silverlight bridge (provided as
IceBridge.ashx), which receives requests from Ice for Silverlight
applications, forward the requests to the appropriate Ice servers, and
then returns the responses to the Ice for Silverlight application. It
must be deployed on the same web server as the Ice for Silverlight
application.


Slice-to-Silverlight translator
--------------------------------

The Slice-to-Silverlight translator (slice2sl.exe) is included in the
bin directory. Its purpose is to translate Slice definitions into Ice
for Silverlight code that you compile into your application.


Ice for .NET
------------

In order to use Ice for Silverlight, it is necessary to also install
Ice for .NET. Specifically, you must install the Ice for .NET 3.3.0
run time (Ice.dll), which is provided in the bin directory. The
assembly is required by the bridge and must be installed in the GAC
using the following command:

  > gacutil -i Ice.dll


======================================================================
Building and running the demo
======================================================================

Since Ice for Silverlight provides client-side functionality only, you
must use an Ice server from another Ice language mapping in order to
run the hello demo. Any of the Ice 3.3.0 mappings that provide server-
side functionality (C++, Java, C#, Python) can be used. You can
download an Ice distribution here:

  http://www.zeroc.com/download.html

To compile and run the demo, do the following:

1) Start the Visual Studio 2008 IDE and open the solution file
   (demo\Ice\hello\hello.sln). 

2) Select Build->Build Solution to build both projects in the 
   solution.

3) Start the demo server from the Ice 3.3.0 package you downloaded.
   See the README in the Ice 3.3.0 demo directory for information on
   how to start the server.

4) Right click on TestPage.html and choose "Set As Start Page".

5) Right click on the HelloWeb project and select "Debug->Start new
   instance". This should cause a browser window to be spawned.

6) Click on the buttons in the browser to run the demo.
