(function(){
$("#hello").click(
    function()
    {
        //
        // Clear any previous error messages.
        //
        $("#console").val("");
        
        var communicator;
        Ice.Promise.try(
            function()
            {
                //
                // Initialize the communicator.
                //
                communicator = Ice.initialize();
                
                //
                // Create a proxy for the hello object
                //
                var proxy = communicator.stringToProxy(
                                "hello:ws -h " + hostname() + " -p 10000");
                
                //
                // Down-cast this proxy to the derived interface Demo::Hello
                // using checkedCast, and invoke the sayHello operation if 
                // the checkedCast success.
                //
                return Demo.HelloPrx.checkedCast(proxy).then(
                    function(r, hello)
                    {
                        return hello.sayHello();
                    });
            }
        ).finally(
            function()
            {
                //
                // Destroy the communicator in a finally block to
                // ensure is always done.
                //
                if(communicator)
                {
                    communicator.destroy();
                }
            }
        ).exception(
            function(ex)
            {
                //
                // Handle any exceptions throw above.
                //
                $("#console").val(ex.toString());
            });
        return false;
    });

//
// Helper function to get the hostname
//
var hostname = function()
{
    return $("#hostname").val() || $("#hostname").attr("placeholder");
};

}());