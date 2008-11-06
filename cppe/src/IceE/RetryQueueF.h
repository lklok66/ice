// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_RETRY_QUEUE_F_H
#define ICE_RETRY_QUEUE_F_H

#include <IceE/Handle.h>

#ifdef ICEE_HAS_AMI

namespace IceInternal
{

class RetryQueue;
IceUtil::Shared* upCast(RetryQueue*);
typedef Handle<RetryQueue> RetryQueuePtr;

}

#endif

#endif
