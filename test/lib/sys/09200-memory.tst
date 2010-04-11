
/*
 *  Test the Memory class
 */

if (Config.OS == "MACOSX" || Config.OS == "LINUX") {
    allocated = Memory.allocated
    assert(allocated > 0) 
    assert(allocated < Memory.system) 
    assert(allocated <= Memory.peak) 
    assert(allocated < Memory.maximum) 

/*
    redline = Memory.redline
    assert(redline > 0) 
    assert(redline < Memory.system) 
    assert(redline < Memory.maximum) 
*/

    resident = Memory.allocated
    assert(resident > 0) 
    assert(resident < Memory.system) 
    assert(resident < Memory.maximum) 

    stack = Memory.allocated
    assert(stack > 0) 
    assert(stack < Memory.system) 

/*
 *  This is working but the redline is very twitchy
 *
    trapError = false
    Memory.redline = 4 * 1024 * 1024
    Memory.callback = function(size, total) {
        Memory.redline = 0
        trapError = true
        // print("TRAPPED")
    }

    var start = Memory.allocated
    a = []
    for (i in 10000) {
        a[i] = o = {}
        o.a = i + 1
        o.b = i + 2
        o.c = i + 3
    }
    delete a
    assert(trapError == true)
*/
}

/*
    Test memory redline getter / setter
 */
assert(Memory.maximum > 0)
assert(Memory.redline < Memory.maximum)

v = 2147483647
Memory.maximum = v
assert(Memory.maximum == v)
Memory.maximum = -1

v = 10000000
Memory.redline = v
assert(Memory.redline == v)
Memory.redline = -1
