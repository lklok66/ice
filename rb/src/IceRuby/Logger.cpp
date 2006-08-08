// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Logger.h>
#include <Util.h>

using namespace std;
using namespace IceRuby;

static VALUE _loggerClass;

extern "C"
void
IceRuby_Logger_free(Ice::LoggerPtr* p)
{
    assert(p);
    delete p;
}

VALUE
IceRuby::createLogger(const Ice::LoggerPtr& p)
{
    return Data_Wrap_Struct(_loggerClass, 0, IceRuby_Logger_free, new Ice::LoggerPtr(p));
}

extern "C"
VALUE
IceRuby_Logger_print(VALUE self, VALUE message)
{
    ICE_RUBY_TRY
    {
	Ice::LoggerPtr* p = reinterpret_cast<Ice::LoggerPtr*>(DATA_PTR(self));
	assert(p);

	string msg = getString(message);
	(*p)->print(msg);
    }
    ICE_RUBY_CATCH
    return Qnil;
}

extern "C"
VALUE
IceRuby_Logger_trace(VALUE self, VALUE category, VALUE message)
{
    ICE_RUBY_TRY
    {
	Ice::LoggerPtr* p = reinterpret_cast<Ice::LoggerPtr*>(DATA_PTR(self));
	assert(p);

	string cat = getString(category);
	string msg = getString(message);
	(*p)->trace(cat, msg);
    }
    ICE_RUBY_CATCH
    return Qnil;
}

extern "C"
VALUE
IceRuby_Logger_warning(VALUE self, VALUE message)
{
    ICE_RUBY_TRY
    {
	Ice::LoggerPtr* p = reinterpret_cast<Ice::LoggerPtr*>(DATA_PTR(self));
	assert(p);

	string msg = getString(message);
	(*p)->warning(msg);
    }
    ICE_RUBY_CATCH
    return Qnil;
}

extern "C"
VALUE
IceRuby_Logger_error(VALUE self, VALUE message)
{
    ICE_RUBY_TRY
    {
	Ice::LoggerPtr* p = reinterpret_cast<Ice::LoggerPtr*>(DATA_PTR(self));
	assert(p);

	string msg = getString(message);
	(*p)->error(msg);
    }
    ICE_RUBY_CATCH
    return Qnil;
}

bool
IceRuby::initLogger(VALUE iceModule)
{
    //
    // Logger.
    //
    _loggerClass = rb_define_class_under(iceModule, "LoggerI", rb_cObject);

    //
    // Instance methods.
    //
    rb_define_method(_loggerClass, "print", CAST_METHOD(IceRuby_Logger_print), 1);
    rb_define_method(_loggerClass, "trace", CAST_METHOD(IceRuby_Logger_trace), 2);
    rb_define_method(_loggerClass, "warning", CAST_METHOD(IceRuby_Logger_warning), 1);
    rb_define_method(_loggerClass, "error", CAST_METHOD(IceRuby_Logger_error), 1);

    return true;
}
