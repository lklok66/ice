
var http = require("http");
var https = require("https");
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

var HttpServer = function(host, ports)
{
    this._host = host;
    this._ports = ports;
};

HttpServer.prototype.start = function()
{
    var baseDir = path.join(__dirname, "../../../certs/winrt");
    var options = {
        passphrase: "password",
        key: fs.readFileSync(path.join(baseDir, "s_rsa1024_priv.pem")),
        cert: fs.readFileSync(path.join(baseDir, "s_rsa1024_pub.pem"))
    };
    
    var servers = [
        http.createServer(),
        https.createServer(options)];
        
    var self = this;

    var requestCB = function(req, res)
        {
            self.processRequest(req, res);
        }

    servers.forEach(
        function(server, i)
        {
            server.on("request", requestCB);
            server.listen(self._ports[i], this._host);
        });
};

var defaultController = new FileServant(path.join(__dirname, "../.."));

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

var server = new HttpServer("0.0.0.0", [8080, 9090]);
server.start();

