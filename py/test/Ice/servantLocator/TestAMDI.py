# **********************************************************************
#
# Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, traceback, time
import Ice, Test

def test(b):
    if not b:
        raise RuntimeError('test assertion failed')

class TestI(Test.TestIntf):

    def requestFailedException_async(self, cb, current=None):
        cb.ice_response()

    def unknownUserException_async(self, cb, current=None):
        cb.ice_response()

    def unknownLocalException_async(self, cb, current=None):
        cb.ice_response()

    def unknownException_async(self, cb, current=None):
        cb.ice_response()

    def localException_async(self, cb, current=None):
        cb.ice_response()

    def userException_async(self, cb, current=None):
        cb.ice_response()

    def pythonException_async(self, cb, current=None):
        cb.ice_response()

    def unknownExceptionWithServantException_async(self, cb, current=None):
        cb.ice_exception(Ice.ObjectNotExistException())

    def impossibleException_async(self, cb, throw, current=None):
        if throw:
            cb.ice_exception(Test.TestImpossibleException())
        else:
            #
            # Return a value so we can be sure that the stream position
            # is reset correctly if finished() throws.
            #
            cb.ice_response("Hello")

    def intfUserException_async(self, cb, throw, current=None):
        if throw:
            cb.ice_exception(Test.TestIntfUserException())
        else:
            #
            # Return a value so we can be sure that the stream position
            # is reset correctly if finished() throws.
            #
            cb.ice_response("Hello")

    def asyncResponse_async(self, cb, current=None):
        cb.ice_response()
        raise Ice.ObjectNotExistException()

    def asyncException_async(self, cb, current=None):
        cb.ice_exception(Test.TestIntfUserException())
        raise Ice.ObjectNotExistException()

    def shutdown_async(self, cb, current=None):
        current.adapter.deactivate()
        cb.ice_response()

class CookieI(Test.Cookie):
    def message(self):
        return 'blahblah'

class ServantLocatorI(Ice.ServantLocator):
    def __init__(self, category):
        self._deactivated = False
        self._category = category
        self._requestId = -1

    def __del__(self):
        test(self._deactivated)

    def locate(self, current):
        test(not self._deactivated)

        test(current.id.category == self._category or self._category == "")
        
        if current.id.name == "unknown":
            return None

        test(current.id.name == "locate" or current.id.name == "finished")
        if current.id.name == "locate":
            self.exception(current)

        #
        # Ensure locate() is only called once per request.
        #
        test(self._requestId == -1)
        self._requestId = current.requestId

        return (TestI(), CookieI())

    def finished(self, current, servant, cookie):
        test(not self._deactivated)

        #
        # Ensure finished() is only called once per request.
        #
        test(self._requestId == current.requestId)
        self._requestId = -1

        test(current.id.category == self._category  or self._category == "")
        test(current.id.name == "locate" or current.id.name == "finished")
        
        if current.id.name == "finished":
            self.exception(current)

        test(isinstance(cookie, Test.Cookie))
        test(cookie.message() == 'blahblah')

    def deactivate(self, category):
        test(not self._deactivated)

        self._deactivated = True

    def exception(self, current):
        if current.operation == "ice_ids":
            raise Test.TestIntfUserException()
        elif current.operation == "requestFailedException":
            raise Ice.ObjectNotExistException()
        elif current.operation == "unknownUserException":
            raise Ice.UnknownUserException("reason")
        elif current.operation == "unknownLocalException":
            raise Ice.UnknownLocalException("reason")
        elif current.operation == "unknownException":
            raise Ice.UnknownException("reason")
        elif current.operation == "userException":
            raise Test.TestIntfUserException()
        elif current.operation == "localException":
            raise Ice.SocketException(0)
        elif current.operation == "pythonException":
            raise RuntimeError("message")
        elif current.operation == "unknownExceptionWithServantException":
            raise Ice.UnknownException("reason")
        elif current.operation == "impossibleException":
            raise Test.TestIntfUserException() # Yes, it really is meant to be TestIntfUserException.
        elif current.operation == "intfUserException":
            raise Test.TestImpossibleException() # Yes, it really is meant to be TestImpossibleException.
        elif current.operation == "asyncResponse":
            raise Test.TestImpossibleException()
        elif current.operation == "asyncException":
            raise Test.TestImpossibleException()
