// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

namespace IceInternal
{
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.IO;
    using System.Net;
    using System.Threading;
    using System;

#if !SILVERLIGHT
    using System.Collections.Specialized;
#endif

    public class Outgoing : OutgoingBase
    {
        public Outgoing(Reference r, string operation, Ice.OperationMode mode,
                        Dictionary<string, string> context)
        {
            _state = StateUnsent;
            _oneway = r.getMode() == Reference.Mode.ModeOneway;

            bridgeUri__ = r.getInstance().bridgeUri();
            logger__ = r.getInstance().initializationData().logger;
            traceLevels__ = r.getInstance().traceLevels();
            is__ = new BasicStream(r.getInstance());
            os__ = new BasicStream(r.getInstance());
#if SILVERLIGHT
            dispatcher__ = r.getInstance().dispatcher();
#endif
            writeHeader__(r, operation, mode, context);
        }

        // Returns true if ok, false if user exception.
        public bool invoke()
        {
            os__.endWriteEncaps();
#if SILVERLIGHT
            if(System.Threading.Thread.CurrentThread.ManagedThreadId == 1)
            {
                throw new Ice.FeatureNotSupportedException("can't send synchronous calls from UI thread.");
            }

            dispatcher__.BeginInvoke(delegate { getRequestStream(); });
#else
            getRequestStream();
#endif
            lock(this)
            {
                while(_state == StateUnsent)
                {
                    Monitor.Wait(this);
                }

                try
                {
                    if(_state == StateOK)
                    {
                        if(_oneway)
                        {
                            return true;
                        }

                        Debug.Assert(_response != null);

                        if(_response.StatusCode != HttpStatusCode.OK)
                        {
                            throw new Ice.ProtocolException("invalid http response code: " + _response.StatusCode);
                        }
    
                        int sz = 12; // Reply header size
                        is__.resize(sz, true);
                        ByteBuffer buf = is__.prepareRead();
                        int position = 0;
                        is__.pos(position);
    
                        try
                        {
                            //
                            // Read the reply header.
                            //
                            Stream s = _response.GetResponseStream();
                            readResponse__(s, buf, sz, ref position);
    
                            //
                            // Determine size and read the rest of the reply.
                            //
                            int remaining = BitConverter.ToInt32(buf.toArray(8, 4), 0);
                            is__.resize(sz + remaining, true);
                            buf = is__.prepareRead();
                            readResponse__(s, buf, remaining, ref position);
                        }
                        catch(IOException ex)
                        {
                            throw new Ice.SocketException(ex);
                        }

                        IceInternal.TraceUtil.traceReply("received reply", is__, logger__, traceLevels__);
        		//IceInternal.TraceUtil.dumpStream(is__);
                        return handleResponse__();
                    }
                    else // StateFailed
                    {
                        Debug.Assert(_exception != null);
                        throw _exception;
                    }
                }
                //      
                // A CloseConnectionException indicates graceful
                // server shutdown, and is therefore always repeatable
                // without violating "at-most-once". That's because by
                // sending a close connection message, the server
                // guarantees that all outstanding requests can safely
                // be repeated.
                //
                // This cannot occur with Ice for SL.
                //
                //catch(Ice.CloseConnectionException ex)
                //{
                //    throw;
                //}
                //
                // An ObjectNotExistException can always be retried as
                // well without violating "at-most-once".
                //
                catch(Ice.ObjectNotExistException)
                {
                    throw;
                }
                    //
                // Throw the exception wrapped in a LocalExceptionWrapper, to
                // indicate that the request cannot be resent without
                // potentially violating the "at-most-once" principle.
                //
                catch(Ice.LocalException ex)
                {
                    throw new LocalExceptionWrapper(ex, false);
                }
                finally
                {
                    // Its not necessary to close both the stream and the response.
                    //s.Close();
                    if(_response != null)
                    {
                        _response.Close();
                    }
                }
            }
        }

        public void getRequestStream()
        {
            lock(this)
            {
                HttpWebRequest req = (HttpWebRequest)WebRequest.Create(bridgeUri__);
                req.Method = "POST";
                req.ContentType = "application/binary";

                if(traceLevels__.network >= 2)
                {
                    string str = "trying to bridge request with " + bridgeUri__.ToString();
                    logger__.trace(traceLevels__.networkCat, str);
                }

                IceInternal.TraceUtil.traceRequest("sending request", os__, logger__, traceLevels__);

                try
                {
                    req.BeginGetRequestStream(new AsyncCallback(requestStreamReady), req);
                }
                catch(WebException ex)
                {
                    _exception = new Ice.ConnectFailedException(ex);
                }

                if(_exception != null)
                {
                    _state = StateFailed;
                    Monitor.Pulse(this);
                }
            }
        }

        public void requestStreamReady(IAsyncResult iar)
        {
            lock(this)
            {
                ByteBuffer buf = os__.prepareWrite();
                HttpWebRequest req = null;

                Stream s = null;
                try
                {
                    req = (HttpWebRequest)iar.AsyncState;
                    s = (Stream)req.EndGetRequestStream(iar);
                    s.Write(buf.rawBytes(), 0, buf.limit());
                }
                catch(WebException ex)
                {
                    _exception =  new Ice.ConnectFailedException(ex);
                }
                catch(IOException ex)
                {
                    _exception =  new Ice.SocketException(ex);
                }
                finally
                {
#if !SILVERLIGHT
                    // The stream must be closed.
                    if(s != null)
                    {
                        s.Close();
                        s = null;
                    }
#endif
                }

                if(_exception == null)
                {
                    if(traceLevels__.network >= 1)
                    {
                        string str = "bridging request with " + bridgeUri__.ToString();
                        logger__.trace(traceLevels__.networkCat, str);
                        if(traceLevels__.network >= 3)
                        {
                            str = "sent " + buf.limit() + " of " + buf.limit() + " bytes";
                            logger__.trace(traceLevels__.networkCat, str);
                        }
                    }

                    try
                    {
                        req.BeginGetResponse(new AsyncCallback(responseReady), req);
                    }
                    catch(WebException ex)
                    {
                        // Error occurred in processing the response. The
                        // request cannot be retried.
                        _exception = new Ice.SocketException(ex);
                    }
                }

                if(_oneway || _exception != null)
                {
                    _state = _exception != null ? StateFailed : StateOK;
                    Monitor.Pulse(this);
                }
            }
        }

        public void responseReady(IAsyncResult iar)
        {
            lock(this)
            {
                try
                {
                    _response = (HttpWebResponse)((HttpWebRequest)iar.AsyncState).EndGetResponse(iar);
                    if(_oneway)
                    {
                        _response.Close();
                        return;
                    }
                }
                catch(WebException ex)
                {
                    // Error occurred in processing the response. The
                    // request cannot be retried.
                    _exception = new Ice.SocketException(ex);
                }

                _state = _exception != null ? StateFailed : StateOK;
                Monitor.Pulse(this);
            }
        }

        public BasicStream istr()
        {
            return is__;
        }
        
        public BasicStream ostr()
        {
            return os__;
        }

        private const int StateUnsent = 0;
        private const int StateOK = 1;
        private const int StateFailed = 3;
        private int _state;

        private bool _oneway;
        private HttpWebResponse _response = null;
        private Ice.Exception _exception = null;
    }
}
