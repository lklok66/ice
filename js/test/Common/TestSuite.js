// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

$(document).foundation();
    $(document).ready(
        function(){
        
            var defaultHost = document.location.host;
            if(defaultHost)
            {
                defaultHost = defaultHost.substr(0, defaultHost.indexOf(":"));
            }
            else
            {
                defaultHost = "127.0.0.1";
            }
            $("#default-host").attr("placeholder", defaultHost);
            
            for(name in TestCases)
            {
                $(".tests").append("<li><a href=\"" + basePath + name + "/index.html\">" + name + "</a></li>");
            }

            var panel = $(".console ul");
            var out = {};
            
            out.write = function(msg){
                var str = $( ".console ul li:last-child" ).text() + msg;
                $(".console ul li:last-child").html(str);
                panel.scrollTop(panel.get(0).scrollHeight);
            };
            
            out.writeLine = function(msg){
                this.write(msg);
                panel.append("<li></li>");
                panel.scrollTop(panel.get(0).scrollHeight);
            };
            
            var enabled = true;
            var runButton = $("#run");
            
            var runCurrentTest = function(){
                var current = $(".tests li.current");
                
                defaultHost = $("#default-host").val() ? $("#default-host").val() : $("#default-host").attr("placeholder");
                var id = new Ice.InitializationData();
                id.properties = Ice.createProperties();
                id.properties.setProperty("Ice.Default.Host", defaultHost);
                id.properties.setProperty("Ice.Default.Protocol", "ws");
                
                __test__(out, id).then(function()
                    {
                        runButton.removeClass("disabled");
                    },
                    function(ex)
                    {
                        out.writeLine("");
                        if(ex && ex._asyncResult)
                        {
                            out.writeLine("exception occurred in call to " + ex._asyncResult.operation);
                        }
                        if(ex.stack)
                        {
                            var lines = ex.stack.split("\n");
                            var line;
                            for(i = 0; i < lines.length; ++i)
                            {
                                line = lines[i];
                                line = line.replace("<", "&lt;");
                                if(i === 0)
                                {
                                    if(line.indexOf(ex.name) === -1)
                                    {
                                        out.writeLine(ex.name + " " + ex.message);
                                        out.writeLine("&nbsp;&nbsp;&nbsp;"  + line);
                                    }
                                    else
                                    {
                                        out.writeLine(line);
                                    }
                                }
                                else
                                {
                                    out.writeLine("&nbsp;&nbsp;&nbsp;"  + line);
                                }
                            }
                        }
                        else
                        {
                            out.writeLine(ex.toString());
                        }
                        runButton.removeClass("disabled");
                    });
            };
            
            runButton.click(function(){
                if(!runButton.hasClass("disabled"))
                {
                    $(".console ul").html("<li></li>");
                    runButton.addClass("disabled");
                    runCurrentTest();
                    return false;
                }
            });
        });