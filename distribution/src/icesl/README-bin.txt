======================================================================
About this distribution
======================================================================

This distribution is a binary release of Ice for Silverlight 0.1.1
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

- Windows XP x86 with Internet Explorer 6.0


Compiler
--------

Ice for Silverlight requires:

- Microsoft Visual Studio 2008

  http://msdn2.microsoft.com/en-us/vstudio/aa700831.aspx

- Microsoft Visual Studio 2008 Silverlight Tools

  http://www.microsoft.com/downloads/details.aspx?FamilyID=e0bae58e-9c0b-4090-a1db-f134d9f095fd&displaylang=en


Silverlight Runtime
-------------------

- Silverlight 2.0 Beta 1

  Downloaded as part of the Visual Studio Tools above.


======================================================================
Package Contents
======================================================================

Ice for Silverlight
-------------------

There are two main components included in the package. First, the 
Ice for Silverlight assembly (icesl.dll) which all Ice for 
Silverlight applications must reference.

Second, the Ice for Silverlight bridge (IceBridge.ashx) which receives 
requests from Ice for Silverlight applications, forward the request
onto the Ice server and then returns the request back to the Ice for
Silverlight application. It must be deployed on the same web server
as the Ice for Silverlight application is running.


Slice-to-Silverlight translator
--------------------------------

The Slice-to-Silverlight translator (slice2sl.exe) is included in the
bin directory. It is required to translate Slice definitions into Ice
for Silverlight code.


Ice for .NET
------------

In order to use Ice for Silverlight it is necessary to also install 
Ice for .NET. Specifically it is necessary to install the Ice for .NET
3.3.0 Ice.dll which is provided in the bin directory. The assembly is
required by the bridge and must be installed in the GAC.

  gacutil -i Ice.dll


======================================================================
Building and running the demo
======================================================================

Since Ice for Silverlight provides client-side functionality only, it 
is required to use the Ice server from another Ice language mapping 
in order to run the hello demo. Any of the Ice 3.3.0 mappings that
provide server-side functionality (C++, Java, C#, Python) can be used. 
You can download one of the other mappings of Ice here:

  http://www.zeroc.com/download.html

Now to compile and run the demo do the following:

1) Start the Visual Studio 2008 IDE and open the solution file
   (demo\Ice\hello\hello.sln). 

2) Select Build->Build Solution to build both projects in the 
   solution.

3) Start the demo server from the Ice 3.3.0 package you downloaded.
   See the README in the Ice 3.3.0 demo directory for information on
   how to start the server.

4) Right click on the TestPage.html and choose "Set As Start Page".

5) Right click on the HelloWeb project and select "Debug->Start new
   instance". This should cause a browser window to be spawned.

6) Click on the buttons in the browser to run the demo.
