// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

public interface Transceiver
{
    boolean initialize(AsyncCallback asyncCallback);

    void close();
    void shutdownReadWrite();

    void write(Buffer buf, AsyncCallback callback);
    
    void read(Buffer buf, AsyncCallback callback);
    
    String type();
    String toString();
    void checkSendSize(Buffer buf, int messageSizeMax);
}
