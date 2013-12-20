

//
// Helper function to merge definitions
//
var merge = function(target, source)
{
    for(var s in source)
    {
        if(target[s] === undefined)
        {
            target[s] = source[s];
        }
        else
        {
            merge(target[s], source[s]);
        }
    }
};

module.exports.merge = merge;