// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(module, name){
    var __m = function(global, module, exports, require){
        require("Ice/Ice");
        var Ice = global.Ice;
        
        require("Test");
        var Test = global.Test;
        
        var AllTests = function(communicator, log)
        {
            this._communicator = communicator;
            this._log = log;
            this._promise = new Ice.Promise();
        };

        AllTests.prototype.start = function()
        {
            this._log.write("testing stringToProxy... ");
            var ref = "test:default -p 12010";
            var base = this._communicator.stringToProxy(ref);
            this.test(base !== null);

            var b1 = this._communicator.stringToProxy("test");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getAdapterId().length === 0 && b1.ice_getFacet().length === 0);
            b1 = this._communicator.stringToProxy("test ");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet().length === 0);
            b1 = this._communicator.stringToProxy(" test ");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet().length === 0);
            b1 = this._communicator.stringToProxy(" test");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet().length === 0);
            b1 = this._communicator.stringToProxy("'test -f facet'");
            this.test(b1.ice_getIdentity().name === "test -f facet" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet().length === 0);
            try
            {
                b1 = this._communicator.stringToProxy("\"test -f facet'");
                this.test(false);
            }
            catch(ex)
            {
                if(!(ex instanceof Ice.ProxyParseException))
                {
                    this.test(false);
                }
            }

            b1 = this._communicator.stringToProxy("\"test -f facet\"");
            this.test(b1.ice_getIdentity().name === "test -f facet" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet().length === 0);
            b1 = this._communicator.stringToProxy("\"test -f facet@test\"");
            this.test(b1.ice_getIdentity().name === "test -f facet@test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet().length === 0);
            b1 = this._communicator.stringToProxy("\"test -f facet@test @test\"");
            this.test(b1.ice_getIdentity().name === "test -f facet@test @test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet().length === 0);
            try
            {
                b1 = this._communicator.stringToProxy("test test");
                this.test(false);
            }
            catch(ex)
            {
                if(!(ex instanceof Ice.ProxyParseException))
                {
                    this.test(false);
                }
            }

            b1 = this._communicator.stringToProxy("test\\040test");
            this.test(b1.ice_getIdentity().name === "test test" && b1.ice_getIdentity().category.length === 0);
            try
            {
                b1 = this._communicator.stringToProxy("test\\777");
                this.test(false);
            }
            catch(ex)
            {
                if(!(ex instanceof Ice.IdentityParseException))
                {
                    this.test(false);
                }
            }
            b1 = this._communicator.stringToProxy("test\\40test");
            this.test(b1.ice_getIdentity().name === "test test");

            // Test some octal and hex corner cases.
            b1 = this._communicator.stringToProxy("test\\4test");
            this.test(b1.ice_getIdentity().name === "test\x04test");
            b1 = this._communicator.stringToProxy("test\\04test");
            this.test(b1.ice_getIdentity().name === "test\x04test");
            b1 = this._communicator.stringToProxy("test\\004test");
            this.test(b1.ice_getIdentity().name === "test\x04test");
            b1 = this._communicator.stringToProxy("test\\1114test");
            this.test(b1.ice_getIdentity().name === "test\x494test");

            b1 = this._communicator.stringToProxy("test\\b\\f\\n\\r\\t\\'\\\"\\\\test");
            this.test(b1.ice_getIdentity().name === "test\b\f\n\r\t\'\"\\test" &&
                b1.ice_getIdentity().category.length === 0);

            b1 = this._communicator.stringToProxy("category/test");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category === "category" &&
                b1.ice_getAdapterId().length === 0);

            b1 = this._communicator.stringToProxy("");
            this.test(b1 === null);
            b1 = this._communicator.stringToProxy("\"\"");
            this.test(b1 === null);
            try
            {
                b1 = this._communicator.stringToProxy("\"\" test"); // Invalid trailing characters.
                this.test(false);
            }
            catch(ex)
            {
                if(!(ex instanceof Ice.ProxyParseException))
                {
                    this.test(false);
                }
            }
            try
            {
                b1 = this._communicator.stringToProxy("test:"); // Missing endpoint.
                this.test(false);
            }
            catch(ex)
            {
                if(!(ex instanceof Ice.EndpointParseException))
                {
                    this.test(false);
                }
            }

            b1 = this._communicator.stringToProxy("test@adapter");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getAdapterId() === "adapter");
            try
            {
                b1 = this._communicator.stringToProxy("id@adapter test");
                this.test(false);
            }
            catch(ex)
            {
                if(!(ex instanceof Ice.ProxyParseException))
                {
                    this.test(false);
                }
            }
            b1 = this._communicator.stringToProxy("category/test@adapter");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category === "category" &&
                b1.ice_getAdapterId() === "adapter");
            b1 = this._communicator.stringToProxy("category/test@adapter:tcp");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category === "category" &&
                b1.ice_getAdapterId() === "adapter:tcp");
            b1 = this._communicator.stringToProxy("'category 1/test'@adapter");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category === "category 1" &&
                b1.ice_getAdapterId() === "adapter");
            b1 = this._communicator.stringToProxy("'category/test 1'@adapter");
            this.test(b1.ice_getIdentity().name === "test 1" && b1.ice_getIdentity().category === "category" &&
                b1.ice_getAdapterId() === "adapter");
            b1 = this._communicator.stringToProxy("'category/test'@'adapter 1'");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category === "category" &&
                b1.ice_getAdapterId() === "adapter 1");
            b1 = this._communicator.stringToProxy("\"category \\/test@foo/test\"@adapter");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category === "category /test@foo" &&
                b1.ice_getAdapterId() === "adapter");
            b1 = this._communicator.stringToProxy("\"category \\/test@foo/test\"@\"adapter:tcp\"");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category === "category /test@foo" &&
                b1.ice_getAdapterId() === "adapter:tcp");

            b1 = this._communicator.stringToProxy("id -f facet");
            this.test(b1.ice_getIdentity().name === "id" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet() === "facet");
            b1 = this._communicator.stringToProxy("id -f 'facet x'");
            this.test(b1.ice_getIdentity().name === "id" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet() === "facet x");
            b1 = this._communicator.stringToProxy("id -f \"facet x\"");
            this.test(b1.ice_getIdentity().name === "id" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet() === "facet x");
            try
            {
                b1 = this._communicator.stringToProxy("id -f \"facet x");
                this.test(false);
            }
            catch(ex)
            {
                if(!(ex instanceof Ice.ProxyParseException))
                {
                    this.test(false);
                }
            }
            try
            {
                b1 = this._communicator.stringToProxy("id -f \'facet x");
                this.test(false);
            }
            catch(ex)
            {
                if(!(ex instanceof Ice.ProxyParseException))
                {
                    this.test(false);
                }
            }
            b1 = this._communicator.stringToProxy("test -f facet:tcp");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet() === "facet" && b1.ice_getAdapterId().length === 0);
            b1 = this._communicator.stringToProxy("test -f \"facet:tcp\"");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet() === "facet:tcp" && b1.ice_getAdapterId().length === 0);
            b1 = this._communicator.stringToProxy("test -f facet@test");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet() === "facet" && b1.ice_getAdapterId() === "test");
            b1 = this._communicator.stringToProxy("test -f 'facet@test'");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet() === "facet@test" && b1.ice_getAdapterId().length === 0);
            b1 = this._communicator.stringToProxy("test -f 'facet@test'@test");
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getFacet() === "facet@test" && b1.ice_getAdapterId() === "test");
            try
            {
                b1 = this._communicator.stringToProxy("test -f facet@test @test");
                this.test(false);
            }
            catch(ex)
            {
                if(!(ex instanceof Ice.ProxyParseException))
                {
                    this.test(false);
                }
            }
            b1 = this._communicator.stringToProxy("test");
            this.test(b1.ice_isTwoway());
            b1 = this._communicator.stringToProxy("test -t");
            this.test(b1.ice_isTwoway());
            b1 = this._communicator.stringToProxy("test -o");
            this.test(b1.ice_isOneway());
            b1 = this._communicator.stringToProxy("test -O");
            this.test(b1.ice_isBatchOneway());
            b1 = this._communicator.stringToProxy("test -d");
            this.test(b1.ice_isDatagram());
            b1 = this._communicator.stringToProxy("test -D");
            this.test(b1.ice_isBatchDatagram());
            b1 = this._communicator.stringToProxy("test");
            this.test(!b1.ice_isSecure());
            b1 = this._communicator.stringToProxy("test -s");
            this.test(b1.ice_isSecure());

            this.test(b1.ice_getEncodingVersion().equals(Ice.currentEncoding()));

            b1 = this._communicator.stringToProxy("test -e 1.0");
            this.test(b1.ice_getEncodingVersion().major === 1 && b1.ice_getEncodingVersion().minor === 0);

            b1 = this._communicator.stringToProxy("test -e 6.5");
            this.test(b1.ice_getEncodingVersion().major === 6 && b1.ice_getEncodingVersion().minor === 5);

            b1 = this._communicator.stringToProxy("test -p 1.0 -e 1.0");
            this.test(b1.toString() === "test -t -e 1.0");

            b1 = this._communicator.stringToProxy("test -p 6.5 -e 1.0");
            this.test(b1.toString() === "test -t -p 6.5 -e 1.0");

            try
            {
                b1 = this._communicator.stringToProxy("test:tcp@adapterId");
                this.test(false);
            }
            catch(ex)
            {
                if(!(ex instanceof Ice.EndpointParseException))
                {
                    this.test(false);
                }
            }
            // This is an unknown endpoint warning, not a parse exception.
            //
            //try
            //{
            //   b1 = this._communicator.stringToProxy("test -f the:facet:tcp");
            //   this.test(false);
            //}
            //catch(ex:Ice.EndpointParseException)
            //{
            //}
            try
            {
                b1 = this._communicator.stringToProxy("test::tcp");
                this.test(false);
            }
            catch(ex)
            {
                if(!(ex instanceof Ice.EndpointParseException))
                {
                    this.test(false);
                }
            }
            this._log.writeLine("ok");

            this._log.write("testing propertyToProxy... ");
            var prop = this._communicator.getProperties();
            var propertyPrefix = "Foo.Proxy";
            prop.setProperty(propertyPrefix, "test:default -p 12010");
            b1 = this._communicator.propertyToProxy(propertyPrefix);
            this.test(b1.ice_getIdentity().name === "test" && b1.ice_getIdentity().category.length === 0 &&
                b1.ice_getAdapterId().length === 0 && b1.ice_getFacet().length === 0);

            var property;

            property = propertyPrefix + ".Locator";
            this.test(b1.ice_getLocator() === null);
            prop.setProperty(property, "locator:default -p 10000");
            b1 = this._communicator.propertyToProxy(propertyPrefix);
            this.test(b1.ice_getLocator() != null && b1.ice_getLocator().ice_getIdentity().name === "locator");
            prop.setProperty(property, "");

            property = propertyPrefix + ".LocatorCacheTimeout";
            this.test(b1.ice_getLocatorCacheTimeout() === -1);
            prop.setProperty(property, "1");
            b1 = this._communicator.propertyToProxy(propertyPrefix);
            this.test(b1.ice_getLocatorCacheTimeout() === 1);
            prop.setProperty(property, "");

            // Now retest with an indirect proxy.
            prop.setProperty(propertyPrefix, "test");
            property = propertyPrefix + ".Locator";
            prop.setProperty(property, "locator:default -p 10000");
            b1 = this._communicator.propertyToProxy(propertyPrefix);
            this.test(b1.ice_getLocator() != null && b1.ice_getLocator().ice_getIdentity().name === "locator");
            prop.setProperty(property, "");

            property = propertyPrefix + ".LocatorCacheTimeout";
            this.test(b1.ice_getLocatorCacheTimeout() === -1);
            prop.setProperty(property, "1");
            b1 = this._communicator.propertyToProxy(propertyPrefix);
            this.test(b1.ice_getLocatorCacheTimeout() === 1);
            prop.setProperty(property, "");

            // This cannot be tested so easily because the property is cached
            // on communicator initialization.
            //
            //prop.setProperty("Ice.Default.LocatorCacheTimeout", "60");
            //b1 = this._communicator.propertyToProxy(propertyPrefix);
            //this.test(b1.ice_getLocatorCacheTimeout() === 60);
            //prop.setProperty("Ice.Default.LocatorCacheTimeout", "");

            prop.setProperty(propertyPrefix, "test:default -p 12010");

            property = propertyPrefix + ".Router";
            this.test(b1.ice_getRouter() === null);
            prop.setProperty(property, "router:default -p 10000");
            b1 = this._communicator.propertyToProxy(propertyPrefix);
            this.test(b1.ice_getRouter() != null && b1.ice_getRouter().ice_getIdentity().name === "router");
            prop.setProperty(property, "");

            property = propertyPrefix + ".PreferSecure";
            this.test(!b1.ice_isPreferSecure());
            prop.setProperty(property, "1");
            b1 = this._communicator.propertyToProxy(propertyPrefix);
            this.test(b1.ice_isPreferSecure());
            prop.setProperty(property, "");

            property = propertyPrefix + ".ConnectionCached";
            this.test(b1.ice_isConnectionCached());
            prop.setProperty(property, "0");
            b1 = this._communicator.propertyToProxy(propertyPrefix);
            this.test(!b1.ice_isConnectionCached());
            prop.setProperty(property, "");

            property = propertyPrefix + ".EndpointSelection";
            this.test(b1.ice_getEndpointSelection() === Ice.EndpointSelectionType.Random);
            prop.setProperty(property, "Random");
            b1 = this._communicator.propertyToProxy(propertyPrefix);
            this.test(b1.ice_getEndpointSelection() === Ice.EndpointSelectionType.Random);
            prop.setProperty(property, "Ordered");
            b1 = this._communicator.propertyToProxy(propertyPrefix);
            this.test(b1.ice_getEndpointSelection() === Ice.EndpointSelectionType.Ordered);
            prop.setProperty(property, "");

            this._log.writeLine("ok");

            this._log.write("testing proxyToProperty... ");

            b1 = this._communicator.stringToProxy("test");
            b1 = b1.ice_connectionCached(true);
            b1 = b1.ice_preferSecure(false);
            b1 = b1.ice_endpointSelection(Ice.EndpointSelectionType.Ordered);
            b1 = b1.ice_locatorCacheTimeout(100);
            b1 = b1.ice_encodingVersion(new Ice.EncodingVersion(1, 0));

            var router = this._communicator.stringToProxy("router");
            router = router.ice_connectionCached(true);
            router = router.ice_preferSecure(true);
            router = router.ice_endpointSelection(Ice.EndpointSelectionType.Random);
            router = router.ice_locatorCacheTimeout(200);

            var locator = this._communicator.stringToProxy("locator");
            locator = locator.ice_connectionCached(false);
            locator = locator.ice_preferSecure(true);
            locator = locator.ice_endpointSelection(Ice.EndpointSelectionType.Random);
            locator = locator.ice_locatorCacheTimeout(300);

            locator = locator.ice_router(Ice.RouterPrx.uncheckedCast(router));
            b1 = b1.ice_locator(Ice.LocatorPrx.uncheckedCast(locator));

            var proxyProps = this._communicator.proxyToProperty(b1, "Test");
            this.test(proxyProps.size === 18);

            this.test(proxyProps.get("Test") === "test -t -e 1.0");
            this.test(proxyProps.get("Test.CollocationOptimized") === "0");
            this.test(proxyProps.get("Test.ConnectionCached") === "1");
            this.test(proxyProps.get("Test.PreferSecure") === "0");
            this.test(proxyProps.get("Test.EndpointSelection") === "Ordered");
            this.test(proxyProps.get("Test.LocatorCacheTimeout") === "100");

            this.test(proxyProps.get("Test.Locator") === "locator -t -e " +
                Ice.encodingVersionToString(Ice.currentEncoding()));
            this.test(proxyProps.get("Test.Locator.CollocationOptimized") === "0");
            this.test(proxyProps.get("Test.Locator.ConnectionCached") === "0");
            this.test(proxyProps.get("Test.Locator.PreferSecure") === "1");
            this.test(proxyProps.get("Test.Locator.EndpointSelection") === "Random");
            this.test(proxyProps.get("Test.Locator.LocatorCacheTimeout") === "300");

            this.test(proxyProps.get("Test.Locator.Router") === "router -t -e " +
                Ice.encodingVersionToString(Ice.currentEncoding()));
            this.test(proxyProps.get("Test.Locator.Router.CollocationOptimized") === "0");
            this.test(proxyProps.get("Test.Locator.Router.ConnectionCached") === "1");
            this.test(proxyProps.get("Test.Locator.Router.PreferSecure") === "1");
            this.test(proxyProps.get("Test.Locator.Router.EndpointSelection") === "Random");
            this.test(proxyProps.get("Test.Locator.Router.LocatorCacheTimeout") === "200");

            this._log.writeLine("ok");

            this._log.write("testing ice_getCommunicator... ");
            this.test(base.ice_getCommunicator() === this._communicator);
            this._log.writeLine("ok");

            this._log.write("testing proxy methods... ");
            this.test(this._communicator.identityToString(
                    base.ice_identity(this._communicator.stringToIdentity("other")).ice_getIdentity()) === "other");
            this.test(base.ice_facet("facet").ice_getFacet() === "facet");
            this.test(base.ice_adapterId("id").ice_getAdapterId() === "id");
            this.test(base.ice_twoway().ice_isTwoway());
            this.test(base.ice_oneway().ice_isOneway());
            this.test(base.ice_batchOneway().ice_isBatchOneway());
            this.test(base.ice_datagram().ice_isDatagram());
            this.test(base.ice_batchDatagram().ice_isBatchDatagram());
            this.test(base.ice_secure(true).ice_isSecure());
            this.test(!base.ice_secure(false).ice_isSecure());
            this.test(base.ice_preferSecure(true).ice_isPreferSecure());
            this.test(!base.ice_preferSecure(false).ice_isPreferSecure());
            this.test(base.ice_encodingVersion(Ice.Encoding_1_0).ice_getEncodingVersion().equals(Ice.Encoding_1_0));
            this.test(base.ice_encodingVersion(Ice.Encoding_1_1).ice_getEncodingVersion().equals(Ice.Encoding_1_1));
            this.test(!base.ice_encodingVersion(Ice.Encoding_1_0).ice_getEncodingVersion().equals(Ice.Encoding_1_1));
            this._log.writeLine("ok");

            this._log.write("testing proxy comparison... ");

            this.test(this._communicator.stringToProxy("foo").equals(this._communicator.stringToProxy("foo")));
            this.test(!this._communicator.stringToProxy("foo").equals(this._communicator.stringToProxy("foo2")));

            var compObj = this._communicator.stringToProxy("foo");

            this.test(compObj.ice_facet("facet").equals(compObj.ice_facet("facet")));
            this.test(!compObj.ice_facet("facet").equals(compObj.ice_facet("facet1")));

            this.test(compObj.ice_oneway().equals(compObj.ice_oneway()));
            this.test(!compObj.ice_oneway().equals(compObj.ice_twoway()));

            this.test(compObj.ice_secure(true).equals(compObj.ice_secure(true)));
            this.test(!compObj.ice_secure(false).equals(compObj.ice_secure(true)));

            this.test(compObj.ice_connectionCached(true).equals(compObj.ice_connectionCached(true)));
            this.test(!compObj.ice_connectionCached(false).equals(compObj.ice_connectionCached(true)));

            this.test(compObj.ice_endpointSelection(Ice.EndpointSelectionType.Random).equals(
                    compObj.ice_endpointSelection(Ice.EndpointSelectionType.Random)));
            this.test(!compObj.ice_endpointSelection(Ice.EndpointSelectionType.Random).equals(
                    compObj.ice_endpointSelection(Ice.EndpointSelectionType.Ordered)));

            this.test(compObj.ice_connectionId("id2").equals(compObj.ice_connectionId("id2")));
            this.test(!compObj.ice_connectionId("id1").equals(compObj.ice_connectionId("id2")));

            this.test(compObj.ice_connectionId("id1").ice_getConnectionId() === "id1");
            this.test(compObj.ice_connectionId("id2").ice_getConnectionId() === "id2");

            this.test(compObj.ice_compress(true).equals(compObj.ice_compress(true)));
            this.test(!compObj.ice_compress(false).equals(compObj.ice_compress(true)));

            this.test(compObj.ice_timeout(20).equals(compObj.ice_timeout(20)));
            this.test(!compObj.ice_timeout(10).equals(compObj.ice_timeout(20)));

            var loc1 = Ice.LocatorPrx.uncheckedCast(this._communicator.stringToProxy("loc1:default -p 10000"));
            var loc2 = Ice.LocatorPrx.uncheckedCast(this._communicator.stringToProxy("loc2:default -p 10000"));
            this.test(compObj.ice_locator(null).equals(compObj.ice_locator(null)));
            this.test(compObj.ice_locator(loc1).equals(compObj.ice_locator(loc1)));
            this.test(!compObj.ice_locator(loc1).equals(compObj.ice_locator(null)));
            this.test(!compObj.ice_locator(null).equals(compObj.ice_locator(loc2)));
            this.test(!compObj.ice_locator(loc1).equals(compObj.ice_locator(loc2)));

            var rtr1 = Ice.RouterPrx.uncheckedCast(this._communicator.stringToProxy("rtr1:default -p 10000"));
            var rtr2 = Ice.RouterPrx.uncheckedCast(this._communicator.stringToProxy("rtr2:default -p 10000"));
            this.test(compObj.ice_router(null).equals(compObj.ice_router(null)));
            this.test(compObj.ice_router(rtr1).equals(compObj.ice_router(rtr1)));
            this.test(!compObj.ice_router(rtr1).equals(compObj.ice_router(null)));
            this.test(!compObj.ice_router(null).equals(compObj.ice_router(rtr2)));
            this.test(!compObj.ice_router(rtr1).equals(compObj.ice_router(rtr2)));

            var ctx1 = new Ice.HashMap();
            ctx1.set("ctx1", "v1");
            var ctx2 = new Ice.HashMap();
            ctx2.set("ctx2", "v2");
            this.test(compObj.ice_context(null).equals(compObj.ice_context(null)));
            this.test(compObj.ice_context(ctx1).equals(compObj.ice_context(ctx1)));
            this.test(!compObj.ice_context(ctx1).equals(compObj.ice_context(null)));
            this.test(!compObj.ice_context(null).equals(compObj.ice_context(ctx2)));
            this.test(!compObj.ice_context(ctx1).equals(compObj.ice_context(ctx2)));

            this.test(compObj.ice_preferSecure(true).equals(compObj.ice_preferSecure(true)));
            this.test(!compObj.ice_preferSecure(true).equals(compObj.ice_preferSecure(false)));

            var compObj1 = this._communicator.stringToProxy("foo:tcp -h 127.0.0.1 -p 10000");
            var compObj2 = this._communicator.stringToProxy("foo:tcp -h 127.0.0.1 -p 10001");
            this.test(!compObj1.equals(compObj2));

            compObj1 = this._communicator.stringToProxy("foo@MyAdapter1");
            compObj2 = this._communicator.stringToProxy("foo@MyAdapter2");
            this.test(!compObj1.equals(compObj2));

            this.test(compObj1.ice_locatorCacheTimeout(20).equals(compObj1.ice_locatorCacheTimeout(20)));
            this.test(!compObj1.ice_locatorCacheTimeout(10).equals(compObj1.ice_locatorCacheTimeout(20)));

            compObj1 = this._communicator.stringToProxy("foo:tcp -h 127.0.0.1 -p 1000");
            compObj2 = this._communicator.stringToProxy("foo@MyAdapter1");
            this.test(!compObj1.equals(compObj2));

            var endpts1 = this._communicator.stringToProxy("foo:tcp -h 127.0.0.1 -p 10000").ice_getEndpoints();
            var endpts2 = this._communicator.stringToProxy("foo:tcp -h 127.0.0.1 -p 10001").ice_getEndpoints();
            this.test(!endpts1[0].equals(endpts2[0]));
            this.test(endpts1[0].equals(this._communicator.stringToProxy("foo:tcp -h 127.0.0.1 -p 10000").ice_getEndpoints()[0]));

            this.test(compObj1.ice_encodingVersion(Ice.Encoding_1_0).equals(compObj1.ice_encodingVersion(Ice.Encoding_1_0)));
            this.test(!compObj1.ice_encodingVersion(Ice.Encoding_1_0).equals(compObj1.ice_encodingVersion(Ice.Encoding_1_1)));

            //
            // TODO: Ideally we should also test comparison of fixed proxies.
            //
            this._log.writeLine("ok");

            this._log.write("testing checked cast... ");
            var self = this;
            Test.MyClassPrx.checkedCast(base).then(
                function(r, prx)
                {
                    self.cast1(base, prx);
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });

            return this._promise;
        };

        AllTests.prototype.cast1 = function(base, cl)
        {
            this.test(cl !== null);
            var self = this;
            Test.MyDerivedClassPrx.checkedCast(cl).then(
                function(r, prx)
                {
                    self.cast2(base, cl, prx);
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });
        };

        AllTests.prototype.cast2 = function(base, cl, derived)
        {
            this.test(derived !== null);
            this.test(cl.equals(base));
            this.test(derived.equals(base));
            this.test(cl.equals(derived));
            this._log.writeLine("ok");

            this._log.write("testing checked cast with context... ");

            var self = this;
            cl.getContext().then(
                function(r, c)
                {
                    self.gotContext1(base, c);
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });
        };

        AllTests.prototype.gotContext1 = function(base, c)
        {
            this.test(c.size === 0);

            c = new Ice.HashMap();
            c.set("one", "hello");
            c.set("two", "world");

            var self = this;
            Test.MyClassPrx.checkedCast(base, undefined, c).then(
                function(r, prx)
                {
                    self.cast3(base, prx, c);
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });
        };

        AllTests.prototype.cast3 = function(base, cl, c)
        {
            var self = this;
            cl.getContext().then(
                function(r, c2)
                {
                    self.gotContext2(c, c2);
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });
        };

        AllTests.prototype.gotContext2 = function(c, c2)
        {
            this.test(c.equals(c2));
            this._log.writeLine("ok");

            this._log.write("testing encoding versioning... ");

            var ref20 = "test -e 2.0:default -p 12010";
            var cl20 = Test.MyClassPrx.uncheckedCast(this._communicator.stringToProxy(ref20));
            var self = this;
            cl20.ice_ping().then(
                function(r)
                {
                    self.test(false);
                }).exception(
                    function(ex)
                    {
                        self.test(ex instanceof Ice.UnsupportedEncodingException);
                        self.encoding1();
                    });
        };

        AllTests.prototype.encoding1 = function()
        {
            var ref10 = "test -e 1.0:default -p 12010";
            var cl10 = Test.MyClassPrx.uncheckedCast(this._communicator.stringToProxy(ref10));
            var self = this;
            cl10.ice_ping().then(
                function(r)
                {
                    self.encoding2(cl10);
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });
        };

        AllTests.prototype.encoding2 = function(cl10)
        {
            var self = this;
            cl10.ice_encodingVersion(Ice.Encoding_1_0).ice_ping().then(
                function(r)
                {
                    self.encoding3();
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });
        };

        AllTests.prototype.encoding3 = function()
        {
            // 1.3 isn't supported but since a 1.3 proxy supports 1.1, the
            // call will use the 1.1 encoding
            var ref13 = "test -e 1.3:default -p 12010";
            var cl13 = Test.MyClassPrx.uncheckedCast(this._communicator.stringToProxy(ref13));
            var self = this;
            cl13.ice_ping().then(
                function(r)
                {
                    self.protocol1();
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });
        };

        AllTests.prototype.protocol1 = function()
        {
            // TODO: Port ice_invoke tests?

            this._log.writeLine("ok");

            this._log.write("testing protocol versioning... ");

            var ref20 = "test -p 2.0:default -p 12010";
            var cl20 = Test.MyClassPrx.uncheckedCast(this._communicator.stringToProxy(ref20));
            var self = this;
            cl20.ice_ping().then(
                function(r)
                {
                    self.test(false);
                }).exception(
                    function(ex)
                    {
                        self.test(ex instanceof Ice.UnsupportedProtocolException);
                        self.protocol2();
                    });
        };

        AllTests.prototype.protocol2 = function()
        {
            var ref10 = "test -p 1.0:default -p 12010";
            var cl10 = Test.MyClassPrx.uncheckedCast(this._communicator.stringToProxy(ref10));
            var self = this;
            cl10.ice_ping().then(
                function(r)
                {
                    self.protocol3();
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });
        };

        AllTests.prototype.protocol3 = function()
        {
            // 1.3 isn't supported but since a 1.3 proxy supports 1.1, the
            // call will use the 1.1 encoding
            var ref13 = "test -p 1.3:default -p 12010";
            var cl13 = Test.MyClassPrx.uncheckedCast(this._communicator.stringToProxy(ref13));
            var self = this;
            cl13.ice_ping().then(
                function(r)
                {
                    self.opaque();
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });
        };

        AllTests.prototype.opaque = function()
        {
            this._log.writeLine("ok");

            this._log.write("testing opaque endpoints... ");

            try
            {
                // Invalid -x option
                this._communicator.stringToProxy("id:opaque -t 99 -v abc -x abc");
                this.test(false);
            }
            catch(ex)
            {
                this.test(ex instanceof Ice.EndpointParseException);
            }

            try
            {
                // Missing -t and -v
                this._communicator.stringToProxy("id:opaque");
                this.test(false);
            }
            catch(ex)
            {
                this.test(ex instanceof Ice.EndpointParseException);
            }

            try
            {
                // Repeated -t
                this._communicator.stringToProxy("id:opaque -t 1 -t 1 -v abc");
                this.test(false);
            }
            catch(ex)
            {
                this.test(ex instanceof Ice.EndpointParseException);
            }

            try
            {
                // Repeated -v
                this._communicator.stringToProxy("id:opaque -t 1 -v abc -v abc");
                this.test(false);
            }
            catch(ex)
            {
                this.test(ex instanceof Ice.EndpointParseException);
            }

            try
            {
                // Missing -t
                this._communicator.stringToProxy("id:opaque -v abc");
                this.test(false);
            }
            catch(ex)
            {
                this.test(ex instanceof Ice.EndpointParseException);
            }

            try
            {
                // Missing -v
                this._communicator.stringToProxy("id:opaque -t 1");
                this.test(false);
            }
            catch(ex)
            {
                this.test(ex instanceof Ice.EndpointParseException);
            }

            try
            {
                // Missing arg for -t
                this._communicator.stringToProxy("id:opaque -t -v abc");
                this.test(false);
            }
            catch(ex)
            {
                this.test(ex instanceof Ice.EndpointParseException);
            }

            try
            {
                // Missing arg for -v
                this._communicator.stringToProxy("id:opaque -t 1 -v");
                this.test(false);
            }
            catch(ex)
            {
                this.test(ex instanceof Ice.EndpointParseException);
            }

            try
            {
                // Not a number for -t
                this._communicator.stringToProxy("id:opaque -t x -v abc");
                this.test(false);
            }
            catch(ex)
            {
                this.test(ex instanceof Ice.EndpointParseException);
            }

            try
            {
                // < 0 for -t
                this._communicator.stringToProxy("id:opaque -t -1 -v abc");
                this.test(false);
            }
            catch(ex)
            {
                this.test(ex instanceof Ice.EndpointParseException);
            }

            try
            {
                // Invalid char for -v
                this._communicator.stringToProxy("id:opaque -t 99 -v x?c");
                this.test(false);
            }
            catch(ex)
            {
                this.test(ex instanceof Ice.EndpointParseException);
            }

            // Legal TCP endpoint expressed as opaque endpoint
            var p1 = this._communicator.stringToProxy("test -e 1.1:opaque -e 1.0 -t 1 -v CTEyNy4wLjAuMeouAAAQJwAAAA==");
            var pstr = this._communicator.proxyToString(p1);
            this.test(pstr === "test -t -e 1.1:tcp -h 127.0.0.1 -p 12010 -t 10000");
            
            // Opaque endpoint encoded with 1.1 encoding.
            var p2 = this._communicator.stringToProxy("test:opaque -e 1.1 -t 1 -v CTEyNy4wLjAuMeouAAAQJwAAAA==");
            this.test(this._communicator.proxyToString(p2) === "test -t -e 1.1:tcp -h 127.0.0.1 -p 12010 -t 10000");

            if(this._communicator.getProperties().getPropertyAsInt("Ice.IPv6") === 0)
            {
                var ref = "test:default -p 12010";
                var base = this._communicator.stringToProxy(ref);

                var ssl = this._communicator.getProperties().getProperty("Ice.Default.Protocol") === "ssl";
                /* TODO: p1 contains 127.0.0.1 - OK to invoke?
                if(!ssl)
                {
                    p1.ice_encodingVersion(Ice.Util.Encoding_1_0).ice_ping();
                }
                */

                // Two legal TCP endpoints expressed as opaque endpoints
                p1 = this._communicator.stringToProxy("test -e 1.0:opaque -e 1.0 -t 1 -v CTEyNy4wLjAuMeouAAAQJwAAAA==:opaque -e 1.0 -t 1 -v CTEyNy4wLjAuMusuAAAQJwAAAA==");
                pstr = this._communicator.proxyToString(p1);
                this.test(pstr === "test -t -e 1.0:tcp -h 127.0.0.1 -p 12010 -t 10000:tcp -h 127.0.0.2 -p 12011 -t 10000");

                //
                // Test that an SSL endpoint and a nonsense endpoint get
                // written back out as an opaque endpoint.
                //
                p1 = this._communicator.stringToProxy("test -e 1.0:opaque -e 1.0 -t 2 -v CTEyNy4wLjAuMREnAAD/////AA==:opaque -t 99 -e 1.0 -v abch");
                pstr = this._communicator.proxyToString(p1);
                if(!ssl)
                {
                    this.test(pstr === "test -t -e 1.0:opaque -t 2 -e 1.0 -v CTEyNy4wLjAuMREnAAD/////AA==:opaque -t 99 -e 1.0 -v abch");
                }
                else
                {
                    this.test(pstr === "test -t -e 1.0:ssl -h 127.0.0.1 -p 10001:opaque -t 99 -e 1.0 -v abch");
                }

                //
                // Try to invoke on the SSL endpoint to verify that we get a
                // NoEndpointException (or ConnectFailedException when
                // running with SSL).
                //
                var self = this;
                p1.ice_encodingVersion(Ice.Encoding_1_0).ice_ping().then(
                    function(r)
                    {
                        self.test(false);
                    }).exception(
                        function(ex)
                        {
                            try
                            {
                                if(ex instanceof Ice.NoEndpointException)
                                {
                                    self.test(!ssl);
                                }
                                else if(ex instanceof Ice.ConnectFailedException)
                                {
                                    self.test(ssl);
                                }
                                else
                                {
                                    self.exception(ex);
                                    return;
                                }
                                self.echo(p1, ssl);
                            }
                            catch(e)
                            {
                                self.exception(e);
                            }
                        });
            }
            else
            {
                this.finished();
            }
        };

        AllTests.prototype.echo = function(p1, ssl)
        {
            //
            // Test that the proxy with an SSL endpoint and a nonsense
            // endpoint (which the server doesn't understand either) can
            // be sent over the wire and returned by the server without
            // losing the opaque endpoints.
            //
            var ref = "test:default -p 12010";
            var base = this._communicator.stringToProxy(ref);
            var derived = Test.MyDerivedClassPrx.uncheckedCast(base);
            var self = this;
            derived.echo(p1).then(
                function(r, p2)
                {
                    self.echo1(p2, ssl, derived);
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });
        };

        AllTests.prototype.echo1 = function(p2, ssl, derived)
        {
            var pstr = this._communicator.proxyToString(p2);
            if(!ssl)
            {
                this.test(pstr === "test -t -e 1.0:opaque -t 2 -e 1.0 -v CTEyNy4wLjAuMREnAAD/////AA==:opaque -t 99 -e 1.0 -v abch");
            }
            else
            {
                this.test(pstr === "test -t -e 1.0:ssl -h 127.0.0.1 -p 10001:opaque -t 99 -e 1.0 -v abch");
            }

            var self = this;
            derived.shutdown().then(
                function(r)
                {
                    self.finished();
                }).exception(
                    function(ex)
                    {
                        self.exception(ex);
                    });
        };

        AllTests.prototype.finished = function()
        {
            this._log.writeLine("ok");
            this._promise.succeed();
        };

        AllTests.prototype.exception = function(ex)
        {
            if(ex._asyncResult)
            {
                this._log.writeLine("\nexception occurred in call to " + ex._asyncResult.operation);
            }
            this._promise.fail(ex);
        };

        AllTests.prototype.test = function(b)
        {
            if(!b)
            {
                var e = new Error();
                Error.captureStackTrace(e, Error);
                if(e.stack)
                {
                    this._log.write("test failed here:\n" + e.stack);
                }
                else
                {
                    this._log.write("test failed");
                }
                throw new Error("test failed");
            }
        };

        var run = function(out, id)
        {
            var p = new Ice.Promise();
            setTimeout(
                function()
                {
                    var c = null;
                    try
                    {
                        c = Ice.initialize(id);
                        var allTests = new AllTests(c, out);
                        allTests.start().then(
                            function()
                            {
                                cleanup(c);
                                p.succeed();
                            },
                            function(ex)
                            {
                                p.fail(ex);
                            });
                    }
                    catch(ex)
                    {
                        p.fail(ex);
                    }

                    function cleanup(communicator)
                    {
                        if(communicator !== null)
                        {
                            communicator.destroy().then(
                                function()
                                {
                                    p.succeed();
                                },
                                function(ex)
                                {
                                    console.log(ex);
                                    p.fail(ex);
                                });
                        }
                        else
                        {
                            p.succeed();
                        }
                    }
                });
            return p;
        };
        module.exports.test = module.exports.test || {};
        module.exports.test.Common = module.exports.test.Common || {};
        
        if(module.exports.test.Common.TestSuite !== undefined)
        {
            module.exports.test.Common.TestSuite.add("Ice/proxy", run);
        }
        
        module.exports.test.Ice = module.exports.test.Ice || {};
        module.exports.test.Ice.proxy = {run: run};
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "test/Ice/proxy"));