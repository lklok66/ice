// **********************************************************************
//
// Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

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
            
            $("#run").click(function(){
                if(!$(this).hasClass("disabled"))
                {
                    $("#console").val("");
                    $(this).addClass("disabled");
                    defaultHost = $("#default-host").val() ? $("#default-host").val() : $("#default-host").attr("placeholder");
                    
                    var secure = $("#wss").is(":checked");
                    var id = new Ice.InitializationData();
                    id.properties = Ice.createProperties();
                    id.properties.setProperty("Ice.Default.Host", defaultHost);
                    id.properties.setProperty("Ice.Default.Protocol", secure ? "wss" : "ws");
                    
                    __test__(out, id).then(
                        function()
                        {
                            $("#run").removeClass("disabled");
                        }).exception(
                            function(ex, r)
                            {
                                out.writeLine("");
                                if(r instanceof Ice.AsyncResult)
                                {
                                    out.writeLine("exception occurred in call to " + r.operation);
                                }
                                out.writeLine(ex.toString());
                                $("#run").removeClass("disabled");
                            }
                        );
                    return false;
                }
            });
        });
