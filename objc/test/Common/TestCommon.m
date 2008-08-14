// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <TestCommon.h>

#include <stdarg.h>
#include <stdlib.h>

void
tprintf(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    char buf[1024];
    vprintf(fmt, va);
    buf[sizeof(buf)-1] = '\0';
    va_end(va);
    fflush(stdout);
}

void
testFailed(const char* expr, const char* file, unsigned int line)
{
    tprintf("failed!\n");
    tprintf("%s:%u: assertion `%s' failed\n", file, line, expr);
    abort();
}


