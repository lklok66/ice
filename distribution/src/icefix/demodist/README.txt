Introduction
------------

This folder contains sample programs for IceFIX in C++ and C#.


Table of Contents
-----------------

  1. Building and running the C++ IceFIX demos (Visual Studio 2010)
  2. Building and running the C# IceFIX demos (Visual Studio 2010)


======================================================================
1. Building and running the C++ IceFIX demos (Visual Studio 2010)
======================================================================

The C++ demos are in the demo directory.

Note that the Visual Studio project files require the Ice Visual
Studio Add-In, which is installed automatically by the Ice binary
distribution.

To build the C++ demos, start Visual Studio 2010 and open the solution
demo\demo.sln.

In the Project Property Manager, open the IceFIX property sheet,
select User Macros, and verify that "IceFIXHome" and "QuickFIXHome"
point to your installation directories for IceFIX and QuickFIX,
respectively.

Select the demo you wish to build in the Solution Explorer. Choose a
target configuration (Debug or Release), then right click on the
demo and select "Build".

To run a demo, open a command window and change to the desired demo
subdirectory. Review the README.txt file in the subdirectory for
further instructions.


======================================================================
2. Building and running the C# IceFIX demos (Visual Studio 2010)
======================================================================

The C# demos are in the democs directory.

Note that the Visual Studio project files require the Ice Visual
Studio Add-In, which is installed automatically by the Ice binary
distribution.

Edit config\quickfix.reg and update the path "C:\QuickFIX\lib" to
match the location of your QuickFIX .NET library directory. Next, open
regedit and import this file; on x64 systems you must use the x86
version of regedit. This modification to the registry allows Visual
Studio to locate the QuickFIX .NET assemblies without requiring
additional path names in the project file.

To build the C# demos, start Visual Studio 2010 and open the solution
democs\democs.sln.

Select the demo you wish to build in the Solution Explorer. Choose a
target configuration (Debug or Release), then right click on the
demo and select "Build".

To run a demo, open a command window and change to the desired demo
subdirectory. Review the README.txt file in the subdirectory for
further instructions.
