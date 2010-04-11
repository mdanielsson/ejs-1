/*
 *  Bad exception inside a function with break
 */

var f = function()
{
    while (true) {
        try {
            throw new Error("Demo error")
        }
        catch {
            break
        }
        break
    }
}

f()
/* Was crashing here after the 2nd try */
f()
assert(1)
