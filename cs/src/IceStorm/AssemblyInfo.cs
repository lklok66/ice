// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System.Reflection;
using System.Runtime.CompilerServices;

[assembly: AssemblyTitle("icestormcs")]
[assembly: AssemblyDescription("IceStorm run-time support")]
[assembly: AssemblyCompany("ZeroC, Inc.")]
[assembly: AssemblyConfiguration("")]
[assembly: AssemblyProduct("IceStorm for C#")]
[assembly: AssemblyCopyright("Copyright (c) 2003-2005, ZeroC, Inc.")]
[assembly: AssemblyTrademark("Ice")]
[assembly: AssemblyCulture("")]		
[assembly: AssemblyVersion("2.1.2")]
[assembly: AssemblyDelaySign(false)]
#if __MonoCS__
[assembly: AssemblyKeyFile("IceStormcsKey.snk")] // mcs uses different search algorithm.
#else
[assembly: AssemblyKeyFile(@"..\..\IceStormcsKey.snk")]
#endif
[assembly: AssemblyKeyName("")]
