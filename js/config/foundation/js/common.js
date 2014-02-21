(function(){

$(document).foundation();

$("#timeout").noUiSlider({range: [0, 2500], start: 0, handles: 1});
$("#delay").noUiSlider({range: [0, 2500], start: 0, handles: 1});
$("#progress .icon").spin("small");

//
// Show demo/test README.
//
$("#viewReadme").click(
    function()
    {
        $("#readme-modal").foundation("reveal", "open");
        return false;
    });

//
// Show demo source code.
//
$("#source").load($("#source").attr("data"),
    //
    // Load completed
    //
    function(){
        SyntaxHighlighter.defaults.toolbar = false;
        SyntaxHighlighter.highlight();
        $("#viewSource").click(
            function()
            {
                $("#source-modal").foundation("reveal", "open");
                return false;
            });
    });
}());
