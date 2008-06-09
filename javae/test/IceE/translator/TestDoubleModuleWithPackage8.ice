// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

// Test: use DoubleModuleWithPackage types from (different) double module definitions

#include <DoubleModuleWithPackage.ice>

module T1
{
module T2
{

const ::M1::M2::dmwpEnum dmwpTest8Constant = ::M1::M2::dmwpE1;

struct dmwpTest8Struct
{
    ::M1::M2::dmwpEnum e;
    ::M1::M2::dmwpStruct s;
    ::M1::M2::dmwpStructSeq seq;
    ::M1::M2::dmwpStringStructDict dict;
};

sequence<::M1::M2::dmwpStruct> dmwpTest8StructSeq;

interface dmwpTest8Interface extends ::M1::M2::dmwpInterface {};

exception dmwpTest8Exception extends ::M1::M2::dmwpException
{
    ::M1::M2::dmwpEnum e1;
    ::M1::M2::dmwpStruct s1;
    ::M1::M2::dmwpStructSeq seq1;
    ::M1::M2::dmwpStringStructDict dict1;
};

class dmwpTest8Class extends ::M1::M2::dmwpBaseClass implements ::M1::M2::dmwpBaseInterface
{
    ::M1::M2::dmwpStruct
    dmwpTest8Op1(::M1::M2::dmwpEnum i1,
                 ::M1::M2::dmwpStruct i2,
                 ::M1::M2::dmwpStructSeq i3,
                 ::M1::M2::dmwpStringStructDict i4,
                 out ::M1::M2::dmwpEnum o1,
                 out ::M1::M2::dmwpStruct o2,
                 out ::M1::M2::dmwpStructSeq o3,
                 out ::M1::M2::dmwpStringStructDict o4)
        throws ::M1::M2::dmwpException;

    ["ami"]
    ::M1::M2::dmwpStruct
    dmwpTest8Op2(::M1::M2::dmwpEnum i1,
                 ::M1::M2::dmwpStruct i2,
                 ::M1::M2::dmwpStructSeq i3,
                 ::M1::M2::dmwpStringStructDict i4,
                 out ::M1::M2::dmwpEnum o1,
                 out ::M1::M2::dmwpStruct o2,
                 out ::M1::M2::dmwpStructSeq o3,
                 out ::M1::M2::dmwpStringStructDict o4)
        throws ::M1::M2::dmwpException;

    ["amd"]
    ::M1::M2::dmwpStruct
    dmwpTest8Op3(::M1::M2::dmwpEnum i1,
                 ::M1::M2::dmwpStruct i2,
                 ::M1::M2::dmwpStructSeq i3,
                 ::M1::M2::dmwpStringStructDict i4,
                 out ::M1::M2::dmwpEnum o1,
                 out ::M1::M2::dmwpStruct o2,
                 out ::M1::M2::dmwpStructSeq o3,
                 out ::M1::M2::dmwpStringStructDict o4)
        throws ::M1::M2::dmwpException;
};

};
};
