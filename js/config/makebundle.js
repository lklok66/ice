// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

var fs = require('fs');
var path = require('path');
var esprima = require('esprima');
var baseDir = process.argv[2];

var Depends = function()
{
    this.depends = [];
};

Depends.prototype.get = function(file)
{
    for(var i = 0; i < this.depends.length; ++i)
    {
        var obj = this.depends[i];
        if(obj.file === file)
        {
            return obj.depends;
        }
    }
    return [];
};

Depends.prototype.expand = function(o)
{
    if(o === undefined)
    {
        for(i = 0; i < this.depends.length; ++i)
        {
            this.expand(this.depends[i]);
        }
    }
    else
    {
        var newDepends = o.depends.slice();
        for(var j = 0; j < o.depends.length; ++j)
        {
            var depends = this.get(o.depends[j]);
            for(var k = 0; k < depends.length; ++k)
            {
                if(newDepends.indexOf(depends[k]) === -1)
                {
                    newDepends.push(depends[k]);
                }
            }
        }
        
        if(o.depends.length != newDepends.length)
        {

            o.depends = newDepends;
            this.expand(o);
        }
    }
    return this;
}

Depends.comparator = function(a, b)
{
    // B depends on A
    var i;
    for(i = 0; i < b.depends.length; ++i)
    {
        if(b.depends[i] === a.file)
        {
            return -1;
        }
    }
    // A depends on B
    for(i = 0; i < a.depends.length; ++i)
    {
        if(a.depends[i] === b.file)
        {
            return 1;
        }
    }
    return 0;
};

Depends.prototype.sort = function()
{
    var objects = this.depends.slice();
    for(var i = 0; i < objects.length; ++i)
    {
        for(var j = 0; j < objects.length; ++j)
        {
            if(j === i) {continue; }
            var v = Depends.comparator(objects[i], objects[j]);
            if(v < 0)
            {
                var tmp = objects[j];
                objects[j] = objects[i];
                objects[i] = tmp;
            }
        }
    }
    return objects;
};

var Parser = {};

Parser.traverse = function(object, depend, file, basedir)
{
    for(key in object)
    {
        var value = object[key];
        if(value !== null && typeof value == "object") 
        {
            Parser.traverse(value, depend, file, basedir);

            if(value.type === "CallExpression")
            {
                if(value.callee.name === "require")
                {
                    var includedFile = value.arguments[0].value + ".js";
                    if(includedFile.indexOf("Ice/") === 0 ||
                       includedFile.indexOf("IceWS/") === 0)
                    {
                        if(depend.depends.indexOf(includedFile) === -1)
                        {
                            depend.depends.push(includedFile);
                        }
                    }
                }
            }
        }
    }
};

Parser.dir = function(baseDir, depends)
{
    var files = fs.readdirSync(baseDir);
    var d = depends || new Depends();
    for(var i = 0; i < files.length; ++i)
    {
        var file = files[i];
        file = path.join(baseDir, file);
        var stats = fs.statSync(file);
        if(path.extname(file) == ".js" && stats.isFile())
        {
            try
            {
                var dirname = path.basename(path.dirname(file));
                var basename = path.basename(file);
                var depend = { file: dirname + "/" + basename, depends: []};
                d.depends.push(depend);
                var ast = esprima.parse(fs.readFileSync(file, 'utf-8'));
                Parser.traverse(ast, depend, basename, dirname);
            }
            catch(e)
            {
                throw e;
            }
        }
        else if(path.basename(file) !== "browser" && stats.isDirectory())
        {
            Parser.dir(path.join(file), d);
        }
    }
    return d;
};

var d = Parser.dir(baseDir);
d.depends = d.expand().sort();

//
// Define all modules inside an anonymous function we don't want
// that all variables become global in the browser.
//
process.stdout.write("(function()\n");
process.stdout.write("{\n");

//
// Now write a bundle to stdout.
//

var data = fs.readFileSync(path.join(__dirname, "module.js"));
var lines = data.toString().split("\n");
var j;

for(j in lines)
{
    process.stdout.write("    " + lines[j] + "\n");
}
process.stdout.write("\n");

var file, i, length = d.depends.length, fullPath;

for(i = 0;  i < length; ++i)
{
    file = d.depends[i].file;
    fullPath = path.join(baseDir, path.dirname(file), "browser",  path.basename(file));
    if(!fs.existsSync(fullPath))
    {
        fullPath = path.join(baseDir, file)
    }
    
    data = fs.readFileSync(fullPath); 
    lines = data.toString().split("\n");
    for(j in lines)
    {
        process.stdout.write("    " + lines[j] + "\n");
    }
}

process.stdout.write("}());");