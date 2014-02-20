(function(){

$(document).foundation();

$("#hostname").attr("placeholder", document.location.hostname || "127.0.0.1");

$("#timeout").noUiSlider({range: [0, 2500], start: 0, handles: 1});
$("#delay").noUiSlider({range: [0, 2500], start: 0, handles: 1});
$("#progress .icon").spin("small");

$("#viewReadme").click(
    function()
    {
        $("#readme-modal").foundation("reveal", "open");
        return false;
    });

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
