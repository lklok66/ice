// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

//
// Compute hash value for float/double values.
//
static inline NSUInteger
ICEInternalHashFloat(float f)
{
  return *(unsigned int*)&f; 
}

static inline NSUInteger
ICEInternalHashDouble(double d)
{
#ifdef __LP64__
  return *(unsigned long*)&d;
#else
  return (*(unsigned int*)&d << 1) ^ *((unsigned int*)&d + 1);
#endif 
}
