// **********************************************************************
//
// Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var communicator = Ice.initialize();

$(document).foundation();
    $(document).ready(
        function(){
        
            $("#default-host").attr("placeholder", document.location.hostname || "127.0.0.1");
            
            $("#console").height(120);
            
            for(name in TestCases)
            {
                $(".tests").append("<li><a href=\"" + basePath + name + "/index.html\">" + name + "</a></li>");
            }
            
            if(document.location.protocol == "https:")
            {
                $("#wss").prop("checked", true);
            }
            else
            {
                $("#wss").prop("disabled", true);
            }

            var out = 
            {
                write: function(msg)
                {
                    var text = $("#console").val();
                    $("#console").val((text == "") ? msg : (text + msg));
                },
                writeLine: function(msg)
                {
                    out.write(msg + "\n");
                    $("#console").scrollTop($("#console").get(0).scrollHeight);
                }
            };
            
            var protocol;
            
            $("#run").click(function(){
                if(!$(this).hasClass("disabled"))
                {
                    $("#console").val("");
                    $(this).addClass("disabled");
                    $("#protocol").prop("disabled", "disabled");
                    defaultHost = $("#default-host").val() ? $("#default-host").val() : $("#default-host").attr("placeholder");
                    
                    protocol = $("#protocol").val() ? "wss" : "ws";
                    var id = new Ice.InitializationData();
                    id.properties = Ice.createProperties();
                    id.properties.setProperty("Ice.Default.Host", defaultHost);
                    id.properties.setProperty("Ice.Default.Protocol", protocol);

                    var str = "controller:ws -h " + defaultHost + " -p 12009";
                    var controller = Test.ControllerPrx.uncheckedCast(communicator.stringToProxy(str));
                    
                    var p;
                    var server;
                    if(typeof(__runServer__) !== "undefined" || typeof(__runEchoServer__) !== "undefined")
                    {
                        if(typeof(__runEchoServer__) !== "undefined")
                        {
                            srv = "Ice/echo"
                        }
                        else
                        {
                            srv = current
                        }
                        out.write("starting " + srv + " server... ");
                        p = controller.runServer("cpp", srv, protocol).then(
                            function(proxy)
                            {
                                out.writeLine("ok");
                                server = proxy;
                                return __test__(out, id);
                            },
                            function(ex)
                            {
                                out.writeLine("failed! (" + ex.ice_name() + ")");
                                return __test__(out, id);
                            }
                        ).then(
                            function()
                            {
                                if(server)
                                {
                                    server.waitTestSuccess();
                                }
                            }
                        ).exception(
                            function(ex)
                            {
                                if(server)
                                {
                                    server.terminate();
                                }
                                throw ex;
                            }
                        );
                    }
                    else
                    {
                        p = __test__(out, id);
                    }
                    
                    return p.finally(
                        function()
                        {
                            $("#protocol").prop("disabled", false);
                            $("#run").removeClass("disabled");
                        }
                    ).then(
                        function()
                        {
                            if($("#loop").is(":checked"))
                            {
                                var location = document.location;
                                var href = location.protocol + "//" + location.hostname;
                                if(protocol == "wss")
                                {
                                    href += ":9090";
                                }
                                else
                                {
                                    href += ":8080";
                                }
                                href += location.pathname.replace(current, next) + "?loop=true";
                                document.location.assign(href);
                            }
                        }
                    ).exception(
                        function(ex, r)
                        {
                            out.writeLine("");
                            if(r instanceof Ice.AsyncResult)
                            {
                                out.writeLine("exception occurred in call to " + r.operation);
                            }
                            out.writeLine(ex.toString());
                            if(ex.stack)
                            {
                                out.writeLine(ex.stack);
                            }
                        });
                }
                return false;
            });
            
            $("#viewReadme").click(
                function()
                {
                    $("#readme-modal").foundation("reveal", "open");
                    return false;
                });
            
            //
            // Check if we should start the test loop=true
            //
            (function(){
                
                var href = document.location.href;
                var i = href.indexOf("?");
                var autoStart = i !== -1 && href.substr(i).indexOf("loop=true") !== -1;
                
                if(document.location.protocol.indexOf("https") != -1)
                {
                    $("#protocol").val("wss");
                }
                else
                {
                    $("#protocol").val("ws");
                }
                
                if(autoStart)
                {
                    $("#loop").prop("checked", true);
                    $("#run").click();
                }
            }());
            
            //
            // Protocol
            //
            $("#protocol").on("change", 
                            function(e)
                            {
                                var newProtocol = $(this).val();
                                if(protocol !== newProtocol)
                                {
                                    var href = document.location.href;
                                    var http;
                                    if(newProtocol == "http")
                                    {
                                        href = href.replace("https", "http");
                                        href = href.replace("9090", "8080");
                                    }
                                    else
                                    {
                                        href = href.replace("http", "https");
                                        href = href.replace("8080", "9090");
                                    }
                                    document.location.assign(href);
                                }
                            });
        });
