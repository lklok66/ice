
var http = require("http");
var url = require("url");
var crypto = require("crypto");
var fs = require("fs");
var path = require("path");

var MimeTypes =
{
    css: "text/css",
    html: "text/html",
    jpeg: "image/jpeg",
    jpg: "image/jpeg",
    js: "text/javascript",
    png: "image/png",
};

var FileServant = function(basePath)
{
    this._basePath = path.resolve(basePath);
};

FileServant.prototype.processRequest = function(req, res)
{
    var filePath = path.resolve(path.join(this._basePath, url.parse(req.url).path));

    fs.stat(filePath, 
            function(err, stats)
            {
                if(err)
                {
                    if(err.code === 'ENOENT')
                    {
                        res.writeHead(404);
                        res.end("404 Page Not Found");
                    }
                    else
                    {
                        res.writeHead(500);
                        res.end("500 Internal Server Error");
                    }
                }
                else
                {
                    if(!stats.isFile())
                    {
                        res.writeHead(403);
                        res.end("403 Forbiden");
                    }
                    else
                    {
                        var ext = path.extname(filePath).slice(1);
                        var headers = 
                        {
                            "Content-Type": MimeTypes[ext] ? MimeTypes[ext] : "text/plain",
                            "Content-Length": stats.size
                        };
                        res.writeHead(200, headers);
                        if(req.method === 'HEAD')
                        {
                            res.end();
                        }
                        else
                        {
                            fs.createReadStream(filePath, { 'bufferSize': 4 * 1024 }).pipe(res);
                        }
                    }
                }
            });
};

var HttpServer = function(host, port)
{
    this._host = host;
    this._port = port;
    this._server = http.createServer();
    this._controllers = {};
};

HttpServer.prototype.start = function()
{
    var self = this;
    this._defaultController = this._controllers[""];
    var requestCB = function(req, res)
        {
            self.processRequest(req, res);
        }

    this._server.on("request", requestCB);
    this._server.listen(this._port, this._host);
};

var defaultController = new FileServant("../..");

HttpServer.prototype.processRequest = function(req, res)
{
    var self = this;
    
    var dataCB = function(data)
        {
        };
    
    var endCB = function()
        {
            req.url = url.parse(req.url);
            console.log(req.url.path);
            defaultController.processRequest(req, res);
        };
    
    req.on("data", dataCB);
    req.on("end", endCB);
};

var server = new HttpServer("0.0.0.0", 8080);
server.start();

