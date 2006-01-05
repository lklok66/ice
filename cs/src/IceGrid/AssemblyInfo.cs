// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Reflection;
using System.Runtime.CompilerServices;

[assembly: CLSCompliant(true)]

[assembly: AssemblyTitle("icegridcs")]
[assembly: AssemblyDescription("IceGrid run-time support")]
[assembly: AssemblyCompany("ZeroC, Inc.")]
[assembly: AssemblyConfiguration("")]
[assembly: AssemblyProduct("IceGrid for C#")]
[assembly: AssemblyCopyright("Copyright (c) 2003-2005, ZeroC, Inc.")]
[assembly: AssemblyTrademark("Ice")]
[assembly: AssemblyCulture("")]		
[assembly: AssemblyVersion("3.0.1")]
[assembly: AssemblyDelaySign(false)]
#if __MonoCS__
[assembly: AssemblyKeyFile("../../config/IcecsKey.snk")] // mcs uses different search algorithm.
#else
[assembly: AssemblyKeyFile("../../../../config/IcecsKey.snk")]
#endif
[assembly: AssemblyKeyName("")]
