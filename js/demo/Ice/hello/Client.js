
(function(module, name){
    var __m = function(global, module, exports, require){
        
        var Ice = global.Ice || {};
        
        require("Hello");
        
        var Demo = global.Demo || {};

        var Client = function()
        {
        };
        
        Client.prototype.run = function(id)
        {
            this._communicator = Ice.initialize(id);
            
           var hello = Demo.HelloPrx.uncheckedCast(this._communicator.propertyToProxy("Hello.Proxy"));
           
           return hello.sayHello(0);
        };
        
        Client.prototype.destroy = function()
        {
            return this._communicator.destroy();
        };
        
        module.exports.Demo = module.exports.Demo || {};
        module.exports.Demo.Client = Client;
    };
    return (module === undefined) ? this.Ice.__defineModule(__m, name) : 
                                    __m(global, module, module.exports, module.require);
}(typeof module !== "undefined" ? module : undefined, "Hello/Client"));