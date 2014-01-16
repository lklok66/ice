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
            
                    
            TestCases.forEach(
                function(name){
                    var s;
                    if(document.location.pathname.indexOf(name) !== -1)
                    {
                        s = "<li><b class=\"current\" href=\"#\">" + name + "</b></li>";
                    }
                    else
                    {
                        s = "<li><a href=\"" + basePath + name + "/index.html\">" + name + "</a></li>";
                    }
                    $(".tests").append(s);
                });

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
                
                var id = new Ice.InitializationData();
                id.properties = Ice.createProperties();
                id.properties.setProperty("Ice.Default.Host", "127.0.0.1");
                id.properties.setProperty("Ice.Default.Protocol", "ws");
                
                run(out, id).then(function()
                    {
                        runButton.removeClass("disabled");
                    },
                    function(ex)
                    {
                        if(ex && ex._asyncResult)
                        {
                            out.writeLine("exception occurred in call to " + ex._asyncResult.operation);
                        }
                        var lines = ex.stack.split("\n");
                        for(i = 0; i < lines.length; ++i)
                        {
                            if(i === 0)
                            {
                                out.writeLine(lines[i]);
                            }
                            else
                            {
                                out.writeLine("&nbsp;&nbsp;&nbsp;"  + lines[i]);
                            }
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
                }
            });
        });