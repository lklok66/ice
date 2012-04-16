# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice Touch is licensed to you under the terms described in the
# ICE_TOUCH_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ../..
COMPILE_CPP_SDK = yes
libdir          = $(CPP_SDK_DIR)/usr/local/lib
STATICLIBS      = yes
LIBNAME		= $(call mklibname,IceCpp)

TARGETS		= $(libdir)/$(LIBNAME)

include $(top_srcdir)/config/Make.rules

OBJS		:= $(wildcard ../IceUtilCpp/$(PLATFORM_NAME)/*.o) \
		   $(wildcard ../IceCpp/$(PLATFORM_NAME)/*.o) \
	 	   $(wildcard ../Transports/$(PLATFORM_NAME)/*.o) \
	 	   $(wildcard ../IceSSL/$(PLATFORM_NAME)/*.o)

$(libdir)/$(LIBNAME): $(OBJS)
	rm -f $@
	$(call mklib,$@,$(OBJS))

include/Ice/%.h:  ../IceCpp/include/Ice/%.h
	@echo Creating $@ from $<
	cp $< $@

include/IceUtil/%.h:  ../IceCpp/include/IceUtil/%.h
	@echo Creating $@ from $<
	cp $< $@

ice_cpp_include = $(ice_cpp_dir)/include
include/IceUtil/%.h: $(ice_cpp_include)/IceUtil/%.h
	@echo Creating $@ from $<
	cp $< $@

include/Ice/%.h:  $(ice_cpp_include)/Ice/%.h
	@echo Creating $@ from $<
	cp $< $@

$(OBJS): include/IceUtil/ArgVector.h \
		include/IceUtil/AbstractMutex.h \
		include/IceUtil/Cache.h \
		include/IceUtil/Config.h \
		include/IceUtil/Cond.h \
		include/IceUtil/Exception.h \
		include/IceUtil/FileUtil.h \
		include/IceUtil/Functional.h \
		include/IceUtil/Handle.h \
		include/IceUtil/IceUtil.h \
		include/IceUtil/Iterator.h \
		include/IceUtil/Lock.h \
		include/IceUtil/Monitor.h \
		include/IceUtil/Mutex.h \
		include/IceUtil/Random.h \
		include/IceUtil/RWRecMutex.h \
		include/IceUtil/MutexPtrLock.h \
		include/IceUtil/MutexProtocol.h \
		include/IceUtil/RecMutex.h \
		include/IceUtil/StaticMutex.h \
		include/IceUtil/Shared.h \
		include/IceUtil/StringUtil.h \
		include/IceUtil/ScopedArray.h \
		include/IceUtil/Thread.h \
		include/IceUtil/ThreadException.h \
		include/IceUtil/Time.h \
		include/IceUtil/Timer.h \
		include/IceUtil/Unicode.h \
		include/IceUtil/UUID.h \
		include/IceUtil/CountDownLatch.h \
		include/Ice/BasicStream.h \
		include/Ice/Buffer.h \
		include/Ice/CommunicatorAsync.h \
		include/Ice/Config.h \
		include/Ice/ConnectionAsync.h \
		include/Ice/ConnectionIF.h \
		include/Ice/Direct.h \
		include/Ice/Dispatcher.h \
		include/Ice/DispatchInterceptor.h \
		include/Ice/DynamicLibrary.h \
		include/Ice/EndpointFactory.h \
		include/Ice/EndpointIF.h \
		include/Ice/Exception.h \
		include/Ice/FactoryTableInit.h \
		include/Ice/FactoryTable.h \
		include/Ice/Functional.h \
		include/Ice/GCCountMap.h \
		include/Ice/GCShared.h \
		include/Ice/Handle.h \
		include/Ice/IncomingAsync.h \
		include/Ice/IncomingAsyncF.h \
		include/Ice/Incoming.h \
		include/Ice/Initialize.h \
		include/Ice/InstanceF.h \
		include/Ice/LocalObject.h \
		include/Ice/LoggerUtil.h \
		include/Ice/Object.h \
		include/Ice/ObjectF.h \
		include/Ice/OutgoingAsync.h \
		include/Ice/OutgoingAsyncF.h \
		include/Ice/Outgoing.h \
		include/Ice/Protocol.h \
		include/Ice/ProtocolPluginFacade.h \
		include/Ice/Proxy.h \
		include/Ice/ProxyF.h \
		include/Ice/ProxyFactoryF.h \
		include/Ice/ProxyHandle.h \
		include/Ice/ReferenceF.h \
		include/Ice/RequestHandlerF.h \
		include/Ice/ServantManagerF.h \
		include/Ice/SliceChecksums.h \
		include/Ice/Stream.h \
		include/Ice/StreamF.h \
		include/Ice/StringConverter.h \
		include/Ice/LocalObject.h \
		include/Ice/LocalObjectF.h \
		include/Ice/UndefSysMacros.h \
		include/Ice/UserExceptionFactory.h \
		$(patsubst ../IceCpp/include/Ice/%.h,include/Ice/%.h,$(wildcard ../IceCpp/include/Ice/*.h))

depend:: include/IceUtil/ArgVector.h \
		include/IceUtil/AbstractMutex.h \
		include/IceUtil/Config.h \
		include/IceUtil/Cond.h \
		include/IceUtil/Exception.h \
		include/IceUtil/FileUtil.h \
		include/IceUtil/Functional.h \
		include/IceUtil/Handle.h \
		include/IceUtil/IceUtil.h \
		include/IceUtil/Iterator.h \
		include/IceUtil/Lock.h \
		include/IceUtil/Monitor.h \
		include/IceUtil/Mutex.h \
		include/IceUtil/MutexPtrLock.h \
		include/IceUtil/Random.h \
		include/IceUtil/RWRecMutex.h \
		include/IceUtil/MutexProtocol.h \
		include/IceUtil/RecMutex.h \
		include/IceUtil/ScopedArray.h \
		include/IceUtil/StaticMutex.h \
		include/IceUtil/Shared.h \
		include/IceUtil/StringUtil.h \
		include/IceUtil/Thread.h \
		include/IceUtil/ThreadException.h \
		include/IceUtil/Time.h \
		include/IceUtil/Timer.h \
		include/IceUtil/Unicode.h \
		include/IceUtil/UUID.h \
		include/IceUtil/CountDownLatch.h \
		include/Ice/BasicStream.h \
		include/Ice/Buffer.h \
		include/Ice/CommunicatorAsync.h \
		include/Ice/Config.h \
		include/Ice/ConnectionAsync.h \
		include/Ice/ConnectionIF.h \
		include/Ice/Direct.h \
		include/Ice/Dispatcher.h \
		include/Ice/DispatchInterceptor.h \
		include/Ice/DynamicLibrary.h \
		include/Ice/EndpointFactory.h \
		include/Ice/EndpointIF.h \
		include/Ice/Exception.h \
		include/Ice/FactoryTableInit.h \
		include/Ice/FactoryTable.h \
		include/Ice/Functional.h \
		include/Ice/GCCountMap.h \
		include/Ice/GCShared.h \
		include/Ice/Handle.h \
		include/Ice/IncomingAsync.h \
		include/Ice/IncomingAsyncF.h \
		include/Ice/Incoming.h \
		include/Ice/Initialize.h \
		include/Ice/InstanceF.h \
		include/Ice/LocalObject.h \
		include/Ice/LoggerUtil.h \
		include/Ice/Object.h \
		include/Ice/ObjectF.h \
		include/Ice/OutgoingAsync.h \
		include/Ice/OutgoingAsyncF.h \
		include/Ice/Outgoing.h \
		include/Ice/Protocol.h \
		include/Ice/ProtocolPluginFacade.h \
		include/Ice/Proxy.h \
		include/Ice/ProxyF.h \
		include/Ice/ProxyFactoryF.h \
		include/Ice/ProxyHandle.h \
		include/Ice/ReferenceF.h \
		include/Ice/RequestHandlerF.h \
		include/Ice/ServantManagerF.h \
		include/Ice/SliceChecksums.h \
		include/Ice/Stream.h \
		include/Ice/StreamF.h \
		include/Ice/StringConverter.h \
		include/Ice/LocalObject.h \
		include/Ice/LocalObjectF.h \
		include/Ice/UndefSysMacros.h \
		include/Ice/UserExceptionFactory.h \
		$(patsubst ../IceCpp/include/Ice/%.h,include/Ice/%.h,$(wildcard ../IceCpp/include/Ice/*.h))

clean::
	rm -f include/IceUtil/*.h
	rm -f include/Ice/*.h

$(EVERYTHING)::
	@echo "making $@ in $$subdir"; \
	    ( cd include && $(MAKE) $@ ) || exit 1; \

include .depend
