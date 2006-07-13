// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

namespace IceInternal
{

    using System;
    using System.Collections;
    using System.Diagnostics;
    using System.Threading;

    public abstract class OutgoingAsync
    {
        public abstract void ice_exception(Ice.Exception ex);
	
        public void finished__(BasicStream istr)
        {
            lock(_monitor)
            {
                DispatchStatus status;
		
                try
                {
                    is__.swap(istr);
		    
                    status = (DispatchStatus)is__.readByte();
		    
                    switch(status)
                    {
                        case DispatchStatus.DispatchOK:
                        case DispatchStatus.DispatchUserException:
                        {
                            is__.startReadEncaps();
                            break;
                        }
			
                        case DispatchStatus.DispatchObjectNotExist:
                        case DispatchStatus.DispatchFacetNotExist:
                        case DispatchStatus.DispatchOperationNotExist:
                        {
                            Ice.Identity id = new Ice.Identity();
                            id.read__(is__);

                            //
                            // For compatibility with the old FacetPath.
                            //
                            string[] facetPath = is__.readStringSeq();
                            string facet;
                            if(facetPath.Length > 0)
                            {
                                if(facetPath.Length > 1)
                                {
                                    throw new Ice.MarshalException();
                                }
                                facet = facetPath[0];
                            }
                            else
                            {
                                facet = "";
                            }

                            string operation = is__.readString();

                            Ice.RequestFailedException ex = null;
                            switch(status)
                            {
                                case DispatchStatus.DispatchObjectNotExist:
                                {
                                    ex = new Ice.ObjectNotExistException();
                                    break;
                                }
				
                                case DispatchStatus.DispatchFacetNotExist:
                                {
                                    ex = new Ice.FacetNotExistException();
                                    break;
                                }
				
                                case DispatchStatus.DispatchOperationNotExist:
                                {
                                    ex = new Ice.OperationNotExistException();
                                    break;
                                }

                                default:
                                {
                                    Debug.Assert(false);
                                    break;
                                }
                            }

                            ex.id = id;
                            ex.facet = facet;;
                            ex.operation = operation;
                            throw ex;
                        }
			
                        case DispatchStatus.DispatchUnknownException:
                        case DispatchStatus.DispatchUnknownLocalException:
                        case DispatchStatus.DispatchUnknownUserException:
                        {
                            string unknown = is__.readString();

                            Ice.UnknownException ex = null;
                            switch(status)
                            {
                                case DispatchStatus.DispatchUnknownException:
                                {
                                    ex = new Ice.UnknownException();
                                    break;
                                }
                                case DispatchStatus.DispatchUnknownLocalException:
                                {
                                    ex = new Ice.UnknownLocalException();
                                    break;
                                }
                                case DispatchStatus.DispatchUnknownUserException:
                                {
                                    ex = new Ice.UnknownUserException();
                                    break;
                                }
                                default:
                                {
                                    Debug.Assert(false);
                                    break;
                                }
                            }
                            ex.unknown = unknown;
                            throw ex;
                        }

                        default:
                        {
                            throw new Ice.UnknownReplyStatusException();
                        }
                    }
                }
                catch(Ice.LocalException ex)
                {
                    finished__(ex);
                    return;
                }
		    
                Debug.Assert(status == DispatchStatus.DispatchOK || status == DispatchStatus.DispatchUserException);
		
                try
                {
                    response__(status == DispatchStatus.DispatchOK);
                }
                catch(System.Exception ex)
                {
                    warning(ex);
                }
                finally
                {
                    cleanup();
                }
            }
        }
	
        public void finished__(Ice.LocalException exc)
        {
            lock(_monitor)
            {

                if(os__ != null)
                {
                    //
                    // A CloseConnectionException indicates graceful
                    // server shutdown, and is therefore always repeatable
                    // without violating "at-most-once". That's because by
                    // sending a close connection message, the server
                    // guarantees that all outstanding requests can safely
                    // be repeated. Otherwise, we can also retry if the
                    // operation mode is Nonmutating or Idempotent.
                    //
		    // An ObjectNotExistException can always be retried as
		    // well without violating "at-most-once".
		    //	
                    if(_mode == Ice.OperationMode.Nonmutating || _mode == Ice.OperationMode.Idempotent ||
                        exc is Ice.CloseConnectionException || exc is Ice.ObjectNotExistException)
                    {
                        try
                        {
			    _cnt = ((Ice.ObjectPrxHelperBase)_proxy).handleException__(exc, _cnt);
			    send__();
			    return;
                        }
                        catch(Ice.LocalException)
                        {
                        }
                    }
                }
		
                try
                {
                    ice_exception(exc);
                }
                catch(System.Exception ex)
                {
                    warning(ex);
                }
                finally
                {
                    cleanup();
                }
            }
        }
	
        public bool timedOut__()
        {
	    long absoluteTimeoutMillis;

            lock(_timeoutMutex) // MONO bug: Should be WaitOne(), but that's broken under Mono 1.0 for Linux.
	    {
		absoluteTimeoutMillis = _absoluteTimeoutMillis;
	    }

            if(absoluteTimeoutMillis > 0)
            {
                return System.DateTime.Now.Ticks / 10000 >= absoluteTimeoutMillis;
            }
            else
            {
                return false;
            }
        }

        protected void prepare__(Ice.ObjectPrx prx, string operation, Ice.OperationMode mode, Ice.Context context)
        {
            lock(_monitor)
            {

                try
                {
                    //
                    // We must first wait for other requests to finish.
                    //
                    while(os__ != null)
                    {
                        Monitor.Wait(_monitor);
                    }

		    //
		    // Can't call sync via a oneway proxy.
		    //
		    ((Ice.ObjectPrxHelperBase)prx).checkTwowayOnly__(operation);

		    _proxy = prx;
                    _cnt = 0;
                    _mode = mode;

		    Reference rf = ((Ice.ObjectPrxHelperBase)prx).reference__();
                    Debug.Assert(is__ == null);
                    is__ = new BasicStream(rf.getInstance());
                    Debug.Assert(os__ == null);
                    os__ = new BasicStream(rf.getInstance());

		    os__.writeBlob(IceInternal.Protocol.requestHdr);
		    
                    rf.getIdentity().write__(os__);

                    //
                    // For compatibility with the old FacetPath.
                    //
                    string facet = rf.getFacet();
                    if(facet == null || facet.Length == 0)
                    {
                        os__.writeStringSeq(null);
                    }
                    else
                    {
                        string[] facetPath = { facet };
                        os__.writeStringSeq(facetPath);
                    }

                    os__.writeString(operation);

                    os__.writeByte((byte)mode);

                    if(context == null)
                    {
                        os__.writeSize(0);
                    }
                    else
                    {
                        int sz = context.Count;
                        os__.writeSize(sz);
                        if(sz > 0)
                        {
                            foreach(DictionaryEntry e in context)
                            {
                                os__.writeString((string)e.Key);
                                os__.writeString((string)e.Value);
                            }
                        }
                    }
		    
                    os__.startWriteEncaps();
                }
                catch(Ice.LocalException)
                {
                    cleanup();
                    throw;
                }
            }
        }
	
        protected void send__()
        {
            lock(_monitor)
            {
                try
                {
                    while(true)
                    {
			bool comp;
			Ice.ConnectionI con;
			con = ((Ice.ObjectPrxHelperBase)_proxy).getDelegate__().getConnection__(out comp);

                        // MONO bug: Should be WaitOne(), but that's broken under Mono 1.0 for Linux.
			lock(_timeoutMutex)
			{
			    if(con.timeout() >= 0)
			    {
				_absoluteTimeoutMillis = System.DateTime.Now.Ticks / 10000 + con.timeout();
			    }
			    else
			    {
				_absoluteTimeoutMillis = 0;
			    }
			}
			
                        try
                        {
                            con.sendAsyncRequest(os__, this, comp);
			    
                            //
                            // Don't do anything after sendAsyncRequest() returned
                            // without an exception.  I such case, there will be
                            // callbacks, i.e., calls to the finished__()
                            // functions. Since there is no mutex protection, we
                            // cannot modify state here and in such callbacks.
                            //
                            return;
                        }
			catch(LocalExceptionWrapper ex)
			{
			    ((Ice.ObjectPrxHelperBase)_proxy).handleExceptionWrapper__(ex);
			}
                        catch(Ice.LocalException ex)
                        {
			    _cnt = ((Ice.ObjectPrxHelperBase)_proxy).handleException__(ex, _cnt);
                        }
                    }
                }
                catch(Ice.LocalException ex)
                {
                    finished__(ex);
                }
            }
        }

        protected abstract void response__(bool ok);

        private void warning(System.Exception ex)
        {
	    if(os__ != null) // Don't print anything if cleanup() was already called.
	    {
		Reference rf = ((Ice.ObjectPrxHelperBase)_proxy).reference__();
		if(rf.getInstance().initializationData().properties.getPropertyAsIntWithDefault(
										"Ice.Warn.AMICallback", 1) > 0)
		{
		    rf.getInstance().initializationData().logger.warning("exception raised by AMI callback:\n" + ex);
		}
	    }
        }

        private void cleanup()
        {
	    is__ = null;
	    os__ = null;

            Monitor.Pulse(_monitor);
        }
	
        protected BasicStream is__;
        protected BasicStream os__;

        private Ice.ObjectPrx _proxy;
        private int _cnt;
        private Ice.OperationMode _mode;

        private long _absoluteTimeoutMillis;
        Mutex _timeoutMutex = new Mutex();

        object _monitor = new object();
    }

}

namespace Ice
{

    public abstract class AMI_Object_ice_invoke : IceInternal.OutgoingAsync
    {
        public abstract void ice_response(bool ok, byte[] outParams);
        public abstract override void ice_exception(Ice.Exception ex);

        public void invoke__(Ice.ObjectPrx prx, string operation, OperationMode mode,
            byte[] inParams, Ice.Context context)
        {
            try
            {
                prepare__(prx, operation, mode, context);
                os__.writeBlob(inParams);
                os__.endWriteEncaps();
            }
            catch(LocalException ex)
            {
                finished__(ex);
                return;
            }
            send__();
        }

        protected override void response__(bool ok) // ok == true means no user exception.
        {
            byte[] outParams;
            try
            {
                int sz = is__.getReadEncapsSize();
                outParams = is__.readBlob(sz);
            }
            catch(LocalException ex)
            {
	        finished__(ex);
		return;
            }
            ice_response(ok, outParams);
        }
    }

}
