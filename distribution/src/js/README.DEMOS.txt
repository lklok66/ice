Introduction
------------

This folder contains sample programs for IceJS. To build and run the
demos, follow the instructions below.


======================================================================
1. Building the Demos
======================================================================

The demos are in the demo directory.

Note that the Visual Studio project files require the Ice Visual
Studio Add-In, installed as part of the installation of the Ice binary
distribution.

To build the demos, start Visual Studio 2012 and open the solution
demo\demo.sln.

Select your target configuration: Debug or Release, Win32 or x64 (on
supported x64 platforms). Right click on the desired demo in the
Solution Explorer window and select "Build".


======================================================================
2. Running the Demos
======================================================================

Add the "bin" directory of the IceJS binary distribution to your PATH.

For x86 builds:

> set PATH=<dir>\IceJS-0.1.0\bin;%PATH%

For x64 builds:

> set PATH=<dir>\IceJS-0.1.0\bin\x64;%PATH%

To run a demo, open a command window and change to the desired demo
subdirectory. Review the README file if one is present. Type 'server'
to start the server. In another command window, type 'client' to start
the client.
