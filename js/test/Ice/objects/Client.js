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
        var Promise = Ice.Promise;

        var test = function(b)
        {
            if(!b)
            {
                throw new Error("test failed");
            }
        };

        var BI = function()
        {
            Test.B.call(this);
            this._postUnmarshalInvoked = false;
        };

        BI.prototype = new Test.B();
        BI.prototype.constructor = BI;

        BI.prototype.postUnmarshalInvoked = function(current)
        {
            return this._postUnmarshalInvoked;
        };

        BI.prototype.ice_preMarshal = function()
        {
            this.preMarshalInvoked = true;
        };

        BI.prototype.ice_postUnmarshal = function()
        {
            this._postUnmarshalInvoked = true;
        };

        var CI = function()
        {
            Test.C.call(this);
            this._postUnmarshalInvoked = false;
        };

        CI.prototype = new Test.C();
        CI.prototype.constructor = CI;

        CI.prototype.postUnmarshalInvoked = function(current)
        {
            return this._postUnmarshalInvoked;
        };

        CI.prototype.ice_preMarshal = function()
        {
            this.preMarshalInvoked = true;
        };

        CI.prototype.ice_postUnmarshal = function()
        {
            this._postUnmarshalInvoked = true;
        };

        var DI = function()
        {
            Test.D.call(this);
            this._postUnmarshalInvoked = false;
        };

        DI.prototype = new Test.D();

        DI.prototype.postUnmarshalInvoked = function(current)
        {
            return this._postUnmarshalInvoked;
        };

        DI.prototype.ice_preMarshal = function()
        {
            this.preMarshalInvoked = true;
        };

        DI.prototype.ice_postUnmarshal = function()
        {
            this._postUnmarshalInvoked = true;
        };

        var EI = function()
        {
            Test.E.call(this, 1, "hello");
        };

        EI.prototype = new Test.E();

        EI.prototype.constructor = EI;

        EI.prototype.checkValues = function(current)
        {
            return this.i == 1 && this.s == "hello";
        };

        var FI = function(e)
        {
            if(e !== undefined)
            {
                Test.F.call(this, e, e);
            }
        };

        FI.prototype = new Test.F();

        FI.prototype.checkValues = function(current)
        {
            return this.e1 != null && this.e1 === this.e2;
        };

        var HI = function()
        {
            Test.H.call(this);
        };

        HI.prototype = new Test.H();
        HI.prototype.constructor = HI;

        var II = function()
        {
            Test.I.call(this);
        };
        II.prototype = new Test.I();
        II.prototype.constructor = II;

        var JI = function()
        {
            Test.J.call(this);
        };
        JI.prototype = new Test.J();
        JI.prototype.constructor = JI;

        var MyObjectFactory = function()
        {
            Ice.ObjectFactory.call(this);
        };

        MyObjectFactory.prototype = new Ice.ObjectFactory();

        MyObjectFactory.prototype.constructor = MyObjectFactory;

        MyObjectFactory.prototype.create = function(type)
        {
            switch(type)
            {
                case "::Test::B":
                    return new BI();
                case "::Test::C":
                    return new CI();
                case "::Test::D":
                    return new DI();
                case "::Test::E":
                    return new EI();
                case "::Test::F":
                    return new FI();
                case "::Test::I":
                    return new II();
                case "::Test::J":
                    return new JI();
                case "::Test::H":
                    return new HI();
                default:
                    break;
            }
            return null;
        };

        MyObjectFactory.prototype.destroy = function()
        {
        };

        var allTests = function(out, communicator)
        {
            var p = new Promise();

            //
            // re-throw exception so it propagates to final exception
            // handler.
            //
            var exceptionCB = function(ex){ throw ex; };

            setTimeout(function(){
                try
                {
                    var factory = new MyObjectFactory();
                    communicator.addObjectFactory(factory, "::Test::B");
                    communicator.addObjectFactory(factory, "::Test::C");
                    communicator.addObjectFactory(factory, "::Test::D");
                    communicator.addObjectFactory(factory, "::Test::E");
                    communicator.addObjectFactory(factory, "::Test::F");
                    communicator.addObjectFactory(factory, "::Test::I");
                    communicator.addObjectFactory(factory, "::Test::J");
                    communicator.addObjectFactory(factory, "::Test::H");

                    out.write("testing stringToProxy... ");
                    var ref = "initial:default -p 12010";
                    var base = communicator.stringToProxy(ref);
                    test(base !== null);
                    out.writeLine("ok");

                    var initial, b1, b2, c, d, i, j, h;
                    out.write("testing checked cast... ");
                    Test.InitialPrx.checkedCast(base).then(
                        function(asyncResult, obj)
                        {
                            initial = obj;
                            test(initial !== null);
                            test(initial.equals(base));
                            out.writeLine("ok");
                        },
                        exceptionCB
                    ).then(
                        function()
                        {
                            out.write("getting B1... ");
                            return initial.getB1();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, obj)
                        {
                            b1 = obj;
                            test(b1 !== null);
                            out.writeLine("ok");
                            out.write("getting B2... ");
                            return initial.getB2();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, obj)
                        {
                            b2 = obj;
                            test(b2 != null);
                            out.writeLine("ok");
                            out.write("getting C... ");
                            return initial.getC();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, obj)
                        {
                            c = obj;
                            test(c !== null);
                            out.writeLine("ok");
                            out.write("getting D... ");
                            return initial.getD();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, obj)
                        {
                            d = obj;
                            test(d !== null);
                            out.writeLine("ok");
                        },
                        exceptionCB
                    ).then(
                        function()
                        {
                            out.write("checking consistency... ");
                            test(b1 !== b2);
                            //test(b1 != c);
                            //test(b1 != d);
                            //test(b2 != c);
                            //test(b2 != d);
                            //test(c != d);
                            test(b1.theB === b1);
                            test(b1.theC === null);
                            test(b1.theA instanceof Test.B);
                            test(b1.theA.theA === b1.theA);
                            test(b1.theA.theB === b1);
                            test(b1.theA.theC instanceof Test.C);
                            test(b1.theA.theC.theB === b1.theA);

                            test(b1.preMarshalInvoked);
                            test(b1.postUnmarshalInvoked(null));
                            test(b1.theA.preMarshalInvoked);
                            test(b1.theA.postUnmarshalInvoked(null));
                            test(b1.theA.theC.preMarshalInvoked);
                            test(b1.theA.theC.postUnmarshalInvoked(null));

                            // More tests possible for b2 and d, but I think this is already
                            // sufficient.
                            test(b2.theA === b2);
                            test(d.theC === null);
                            out.writeLine("ok");
                            out.write("getting B1, B2, C, and D all at once... ");

                            return initial.getAll();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, b1, b2, c, d)
                        {
                            test(b1);
                            test(b2);
                            test(c);
                            test(d);
                            out.writeLine("ok");

                            out.write("checking consistency... ");
                            test(b1 !== b2);
                            //test(b1 != c);
                            //test(b1 != d);
                            //test(b2 != c);
                            //test(b2 != d);
                            //test(c != d);
                            test(b1.theA === b2);
                            test(b1.theB === b1);
                            test(b1.theC === null);
                            test(b2.theA === b2);
                            test(b2.theB === b1);
                            test(b2.theC === c);
                            test(c.theB === b2);
                            test(d.theA === b1);
                            test(d.theB === b2);
                            test(d.theC === null);
                            test(d.preMarshalInvoked);
                            test(d.postUnmarshalInvoked(null));
                            test(d.theA.preMarshalInvoked);
                            test(d.theA.postUnmarshalInvoked(null));
                            test(d.theB.preMarshalInvoked);
                            test(d.theB.postUnmarshalInvoked(null));
                            test(d.theB.theC.preMarshalInvoked);
                            test(d.theB.theC.postUnmarshalInvoked(null));
                            out.writeLine("ok");

                            out.write("testing protected members... ");
                            return initial.getE();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, e)
                        {
                            test(e.checkValues());
                            return initial.getF();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, f)
                        {
                            test(f.checkValues());
                            test(f.e2.checkValues());
                            out.writeLine("ok");
                            out.write("getting I, J and H... ");
                            return initial.getI();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, obj)
                        {
                            i = obj;
                            test(i);
                            return initial.getJ();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, obj)
                        {
                            j = obj;
                            test(j);
                            return initial.getH();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, obj)
                        {
                            h = obj;
                            test(h);
                            out.write("setting I... ");
                            return initial.setI(i);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return initial.setI(j);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            return initial.setI(h);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            out.writeLine("ok");
                            out.write("testing sequences... ");
                            return initial.opBaseSeq([]);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, retS, outS)
                        {
                            return initial.opBaseSeq([new Test.Base(new Test.S(), "")]);
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, retS, outS)
                        {
                            test(retS.length === 1 && outS.length === 1);
                            out.writeLine("ok");
                            out.write("testing compact ID... ");

                            return initial.getCompact();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult, compact)
                        {
                            test(compact !== null);
                            out.writeLine("ok");
                            out.write("testing UnexpectedObjectException... ");
                            ref = "uoet:default -p 12010";
                            base = communicator.stringToProxy(ref);
                            test(base !== null);
                            var uoet = Test.UnexpectedObjectExceptionTestPrx.uncheckedCast(base);
                            test(uoet !== null);
                            return uoet.op();
                        },
                        exceptionCB
                    ).then(
                        function(asyncResult)
                        {
                            test(false);
                        },
                        function(ex)
                        {
                            if(ex instanceof Ice.UnexpectedObjectException )
                            {
                                test(ex.type == "::Test::AlsoEmpty");
                                test(ex.expectedType == "::Test::Empty");
                            }
                            else
                            {
                                exceptionCB(ex);
                            }
                            out.writeLine("ok");
                            return initial.shutdown();
                        }
                    ).then(
                        function()
                        {
                            p.succeed();
                        },
                        exceptionCB
                    ).exception(
                        function(ex)
                        {
                            p.fail(ex);
                        }
                    );
                }
                catch(ex)
                {
                    p.fail(ex);
                }
            });
            return p;
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
                        allTests(out, c).then(function(){
                                return c.destroy();
                            }).then(function(){
                                p.succeed();
                            }).exception(function(ex){
                                p.fail(ex);
                            });
                    }
                    catch(ex)
                    {
                        p.fail(ex);
                    }
                });
            return p;
        };
        module.exports.run = run;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) :
        __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "test/Ice/objects"));
