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
}());