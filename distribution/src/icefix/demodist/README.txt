Introduction
------------

This folder contains sample programs for build IceFIX applicatinos in
C++ and C#.


Table of Contents
-----------------

  1. Building and running the C++ IceFIX demos (Visual Studio 2010)
  2. Building and running the C# IceFIX demos (Visual Studio 2010)

======================================================================
1. Building and running the C++ IceFIX demos (Visual Studio 2010)
======================================================================

The C++ demos are in the demo directory.

Note that the Visual Studio project files require the Ice Visual
Studio Add-In, installed as part of the installation of the Ice binary
distribution.

To build the C++ demos, start Visual Studio 2010 and open the solution
demo\demo.sln.

In the Project Property Manager, open IceFIX property sheet, select 
User Macros, and review that "IceFIXHome" and "QuickFIXHome" point to
the paths where you install IceFIX and QuickFIX respectively 

Select your target configuration: Debug or Release, Win32 Right click 
on the desired demo in the Solution Explorer window and select "Build".

To run a demo, open a command window and change to the desired demo
subdirectory. Review the README.txt file.

======================================================================
2. Building and running the C# IceFIX demos (Visual Studio 2010)
======================================================================

The C# demos are in the democs directory.

Note that the Visual Studio project files require the Ice Visual
Studio Add-In, installed as part of the installation of the Ice binary
distribution. 

Edit config\quickfix.reg and updat "C:\QuickFIX\lib" to match the 
location of your QuickFIX .NET library directory. Then open regedit 
and import this file, on x64 systems you must use the x86 version of 
regedit. That allows visual studio to locate QuickFIX .NET assemblies
without require to put the paths in the project file.

To build the C# demos, start Visual Studio 2010 and open the solution
democs\democs.sln.

Select your target configuration: Debug or Release, Right click on the 
desired demo in the Solution Explorer window and select "Build".

To run a demo, open a command window and change to the desired demo
subdirectory. Review the README.txt file.
