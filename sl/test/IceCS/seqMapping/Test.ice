// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_ICE
#define TEST_ICE


module Test
{

sequence<byte> AByteS;
["clr:generic:List"] sequence<byte> LByteS;
["clr:collection"] sequence<byte> CByteS;

sequence<bool> ABoolS;
["clr:generic:List"] sequence<bool> LBoolS;
["clr:collection"] sequence<bool> CBoolS;

sequence<short> AShortS;
["clr:generic:List"] sequence<short> LShortS;
["clr:collection"] sequence<short> CShortS;

sequence<int> AIntS;
["clr:generic:List"] sequence<int> LIntS;
["clr:collection"] sequence<int> CIntS;

sequence<long> ALongS;
["clr:generic:List"] sequence<long> LLongS;
["clr:collection"] sequence<long> CLongS;

sequence<float> AFloatS;
["clr:generic:List"] sequence<float> LFloatS;
["clr:collection"] sequence<float> CFloatS;

sequence<double> ADoubleS;
["clr:generic:List"] sequence<double> LDoubleS;
["clr:collection"] sequence<double> CDoubleS;

sequence<string> AStringS;
["clr:generic:List"] sequence<string> LStringS;
["clr:collection"] sequence<string> CStringS;

sequence<Object> AObjectS;
["clr:generic:List"] sequence<Object> LObjectS;
["clr:collection"] sequence<Object> CObjectS;

sequence<Object*> AObjectPrxS;
["clr:generic:List"] sequence<Object*> LObjectPrxS;
["clr:collection"] sequence<Object*> CObjectPrxS;

struct S
{
    int i;
};

sequence<S> AStructS;
["clr:generic:List"] sequence<S> LStructS;
["clr:collection"] sequence<S> CStructS;

class CV
{
    int i;
};

sequence<CV> ACVS;
["clr:generic:List"] sequence<CV> LCVS;
["clr:collection"] sequence<CV> CCVS;

sequence<CV*> ACVPrxS;
["clr:generic:List"] sequence<CV*> LCVPrxS;
["clr:collection"] sequence<CV*> CCVPrxS;

class CR
{
    CV v;
};

sequence<CR> ACRS;
["clr:generic:List"] sequence<CR> LCRS;
["clr:collection"] sequence<CR> CCRS;

enum En { A, B, C };

sequence<En> AEnS;
["clr:generic:List"] sequence<En> LEnS;
["clr:collection"] sequence<En> CEnS;

["clr:generic:Custom"] sequence<int> CustomIntS;
["clr:generic:Custom"] sequence<CV> CustomCVS;

["clr:generic:Custom"] sequence<CustomIntS> CustomIntSS;
["clr:generic:Custom"] sequence<CustomCVS> CustomCVSS;

["ami"] class MyClass
{
    void shutdown();

    AByteS opAByteS(AByteS i, out AByteS o);
    LByteS opLByteS(LByteS i, out LByteS o);
    CByteS opCByteS(CByteS i, out CByteS o);

    ABoolS opABoolS(ABoolS i, out ABoolS o);
    LBoolS opLBoolS(LBoolS i, out LBoolS o);
    CBoolS opCBoolS(CBoolS i, out CBoolS o);

    AShortS opAShortS(AShortS i, out AShortS o);
    LShortS opLShortS(LShortS i, out LShortS o);
    CShortS opCShortS(CShortS i, out CShortS o);

    AIntS opAIntS(AIntS i, out AIntS o);
    LIntS opLIntS(LIntS i, out LIntS o);
    CIntS opCIntS(CIntS i, out CIntS o);

    ALongS opALongS(ALongS i, out ALongS o);
    LLongS opLLongS(LLongS i, out LLongS o);
    CLongS opCLongS(CLongS i, out CLongS o);

    AFloatS opAFloatS(AFloatS i, out AFloatS o);
    LFloatS opLFloatS(LFloatS i, out LFloatS o);
    CFloatS opCFloatS(CFloatS i, out CFloatS o);

    ADoubleS opADoubleS(ADoubleS i, out ADoubleS o);
    LDoubleS opLDoubleS(LDoubleS i, out LDoubleS o);
    CDoubleS opCDoubleS(CDoubleS i, out CDoubleS o);

    AStringS opAStringS(AStringS i, out AStringS o);
    LStringS opLStringS(LStringS i, out LStringS o);
    CStringS opCStringS(CStringS i, out CStringS o);

    AObjectS opAObjectS(AObjectS i, out AObjectS o);
    LObjectS opLObjectS(LObjectS i, out LObjectS o);
    CObjectS opCObjectS(CObjectS i, out CObjectS o);

    AObjectPrxS opAObjectPrxS(AObjectPrxS i, out AObjectPrxS o);
    LObjectPrxS opLObjectPrxS(LObjectPrxS i, out LObjectPrxS o);
    CObjectPrxS opCObjectPrxS(CObjectPrxS i, out CObjectPrxS o);

    AStructS opAStructS(AStructS i, out AStructS o);
    LStructS opLStructS(LStructS i, out LStructS o);
    CStructS opCStructS(CStructS i, out CStructS o);

    ACVS opACVS(ACVS i, out ACVS o);
    LCVS opLCVS(LCVS i, out LCVS o);
    CCVS opCCVS(CCVS i, out CCVS o);

    ACRS opACRS(ACRS i, out ACRS o);
    LCRS opLCRS(LCRS i, out LCRS o);
    CCRS opCCRS(CCRS i, out CCRS o);

    AEnS opAEnS(AEnS i, out AEnS o);
    LEnS opLEnS(LEnS i, out LEnS o);
    CEnS opCEnS(CEnS i, out CEnS o);

    ACVPrxS opACVPrxS(ACVPrxS i, out ACVPrxS o);
    LCVPrxS opLCVPrxS(LCVPrxS i, out LCVPrxS o);
    CCVPrxS opCCVPrxS(CCVPrxS i, out CCVPrxS o);

    CustomIntS opCustomIntS(CustomIntS i, out CustomIntS o);
    CustomCVS opCustomCVS(CustomCVS i, out CustomCVS o);

    CustomIntSS opCustomIntSS(CustomIntSS i, out CustomIntSS o);
    CustomCVSS opCustomCVSS(CustomCVSS i, out CustomCVSS o);

};

};

#endif
