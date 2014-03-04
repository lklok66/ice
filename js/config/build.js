
var fs = require("fs");
var path = require("path");

var iceJsDist = "IceJS-0.1.0";
var iceDist = "Ice-3.5.1";

var defaultInstallLocations = [
    "C:\\Program Files\\ZeroC",
    "C:\\Program Files (x86)\\ZeroC"];

var iceHome = process.env.ICE_HOME;

var iceJsHome = process.env.ICE_JS_HOME;
var useBinDist = process.env.USE_BIN_DIST == "yes";

var srcDist; 
try
{
    srcDist = fs.statSync(path.join(__dirname, "..", "lib")).isDirectory(); 
}
catch(e)
{
}

var slice2js = "slice2js.exe";
var slice2cpp = "slice2cpp.exe";

//
// If this is a source distribution and ICE_HOME isn't set ensure 
// that slice2js has been build.
//
if(srcDist && !useBinDist)
{
    var build;
    try
    {
        build = fs.statSync(path.join(__dirname, "..", "..", "cpp", "bin", slice2js)).isFile()
    }
    catch(e)
    {
    }
    
    if(!build)
    {
        console.error("error Unable to find " + slice2js + " in " + path.join(__dirname, "..", "..", "cpp", "bin") + 
                      ", please verify that the sources has been build or configure ICE_HOME to use a binary distribution.");
        process.exit(1);
    }
}

//
// If this is a demo distribution ensure that ICE_HOME and ICE_JS_HOME are set or installed in their default locations.
//
if(!srcDist || useBinDist)
{
    //
    // if ICE_HOME is not set check if it is install in the default location.
    //
    if(!process.env.ICE_HOME)
    {
        defaultInstallLocations.some(
            function(basePath)
            {
                try
                {
                    if(fs.statSync(path.join(basePath, iceDist, "bin", slice2cpp)).isFile())
                    {
                        iceHome = path.join(basePath, iceDist);
                        return true;
                    }
                }
                catch(e)
                {
                }
                return false;
            });
    }

    if(!iceHome)
    {
        console.error("error Ice not found in the default installation directories ICE_HOME environment\n" +
                      "variable must be set, and point to the Ice installation directory.");
        process.exit(1);
    }
    
    //
    // if ICE_JS_HOME is not set check if it is install in the default location.
    //
    if(!process.env.ICE_JS_HOME)
    {
        defaultInstallLocations.some(
            function(basePath)
            {
                try
                {
                    if(fs.statSync(path.join(basePath, iceJsDist, "bin", slice2js)).isFile())
                    {
                        iceJsHome = path.join(basePath, iceJsDist);
                        return true;
                    }
                }
                catch(e)
                {
                }
                return false;
            });
    }

    if(!iceJsHome)
    {
        console.error("error Ice for JavaScript not found in the default installation directories ICE_JS_HOME environment\n" +
                      "variable must be set, and point to the Ice for JavaScript installation directory.");
        process.exit(1);
    }
}


var sliceDir = iceHome ? path.join(iceHome, "slice") :
                         path.join(__dirname, "..", "..", "slice");
                         
var binDir = iceHome ? path.join(iceJsHome, "bin") :
                       path.join(__dirname, "..", "..", "cpp", "bin");

module.exports.build = function(files, args)
{
    args = args || [];
    function buildFile(file)
    {
        var commandArgs = [];
        
        commandArgs.push("-I" + sliceDir);
        args.forEach(
            function(arg)
            {
                commandArgs.push(arg);
            });
        commandArgs.push(file);
        
        var env = {};
        for(var key in process.env)
        {
            env[key] = process.env[key];
        }
        
        if(env["PATH"])
        {
            env["PATH"] = binDir + path.delimiter + env["PATH"];
        }
        else
        {
            env["PATH"] = binDir;
        }
        var options = {env: env};
        
        console.log(slice2js + " " + commandArgs.join(" "));
        var spawn = require("child_process").spawn;
        var build  = spawn(slice2js, commandArgs, options);
        
        build.stdout.on("data", function(data)
        {
            process.stdout.write(data);
        });

        build.stderr.on("data", function(data)
        {
            process.stderr.write(data);
        });

        build.on("close", function(code)
        {
            if(code != 0)
            {
                process.exit(code);
            }
            else
            {
                if(files.length > 0)
                {
                    buildFile(files.shift());
                }
            }
        });
    }
    buildFile(files.shift());
}
