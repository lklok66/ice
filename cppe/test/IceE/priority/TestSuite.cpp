// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <TestSuite.h>
#include <ThreadPriorityTest.h>
#include <TimerPriorityTest.h>
#include <PriorityInversion.h>

std::list<TestBasePtr> allTests;

void
initializeTestSuite()
{
    allTests.push_back(new ThreadPriorityTest);
    allTests.push_back(new TimerPriorityTest);
    allTests.push_back(new PriorityInversionTest);
}
