// **********************************************************************
//
// Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

(function(){

$(document).foundation();

$("#timeout").noUiSlider({range: [0, 2500], start: 0, handles: 1});
$("#delay").noUiSlider({range: [0, 2500], start: 0, handles: 1});
$("#progress .icon").spin("small");

//
// Show demo/test README modal dialog.
//
$("#viewReadme").click(
    function()
    {
        $("#readme-modal").foundation("reveal", "open");
        return false;
    });

//
// Load the source code and highlight it.
//
$(".source").each(
    function(i, e)
    {
        $.ajax(
            {
                url: $(e).attr("data"), 
                //
                // Use text data type to avoid problems interpreting the data.
                //
                dataType: "text"
            }
        ).done(
            function(data)
            {
                $(e).text(data);
                hljs.highlightBlock(e);
            });
    });

//
// Show source code modal dialog.
//
$("#viewSource").click(
    function()
    {
        $("#source-modal").foundation("reveal", "open");
        return false;
    });

//
// If the demo page was not load from a web server display
// the setup-modal dialog.
//
if(document.location.protocol === "file:")
{
    var paths = document.location.pathname.split("/");
    var i = paths.indexOf("demo");
    if(i == -1) // It's a demo distribution?
    {
        i = paths.indexOf("demojs");
    }
    if(i == -1) // It's a test page?
    {
        i = paths.indexOf("test");
    }
    paths = paths.slice(i);
    var href = "https://127.0.0.1:9090/" + paths.join("/");
    $("#setup-modal a.go").text(href);
    $("#setup-modal a.go").attr("href", href);
    $("#setup-modal").foundation({
        reveal:
        {
            close_on_background_click: false,
            close_on_esc: false
        }
    });
    $("#setup-modal").foundation("reveal", "open");
}


}());


//
// Check if the corresponding generated files can be access, if they
// cannot be access display the build-required-modal otherwhise do 
// nothing.
//
function checkGenerated(files)
{
    var dialog = "<div id=\"build-required-modal\" class=\"reveal-modal\" data-reveal>" +
        "<p>Couldn't find generated file `%FILENAME%'. This is expected if you didn't build the JavaScript demos." +
        "To build the demos refer to the instructions from the " +
        "<a href=\"http://doc.zeroc.com/display/Rel/Ice+for+JavaScript+0.1.0+Release+Notes\">release notes</a>.</p>" +
        "</div>";
    
    var basePath = document.location.pathname;
    basePath = basePath.substr(0, basePath.lastIndexOf("/"));
    
    var error = false;
    files.forEach(
        function(f)
        {
            $.ajax(
                {
                    headers: {method: "HEAD"}, 
                    url: basePath + "/" + f,
                    //
                    // Use text data type to avoid problems interpreting the data.
                    //
                    dataType: "text"
                }
            ).fail(
                function(err)
                {
                    if(!error)
                    {
                        error = true;
                        $("body").append(dialog.replace("%FILENAME%", f));
                        $("#build-required-modal").foundation({
                            reveal:
                            {
                                close_on_background_click: false,
                                close_on_esc: false
                            }
                        });
                        $("#build-required-modal").foundation("reveal", "open");
                    }
                });
        }
    );
}