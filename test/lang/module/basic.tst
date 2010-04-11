/*
 *	Module tests
 *  NOTE: Versioned modules are an experimental feature and likely to change in syntax
 */



/*
 *  NOTE: This code does not execute linearly. As each "require" statement is executed, the corresponding module
 *  initializer will be executed.
 */

var x = "GLOBAL"
assert(x == "GLOBAL")

/* BUG
module {
    x = "DEFAULT"
    assert(x == "DEFAULT")
}
assert(x == "GLOBAL")
*/

var yellow
module yellow {
    var x = "YELLOW"
    assert(x == "YELLOW")
    yellow = true
}
assert(x == "GLOBAL")

var red
module red {
    var x = "RED"
    assert(x == "RED")
    red = true
}
assert(x == "GLOBAL")

var red1
//  Vesioned modules
module red 1 {
    var x = "RED-1"
    assert(x == "RED-1")
    red1 = true

}

assert(x == "GLOBAL")

module red 2 {
    var x = "RED-2"
    assert(x == "RED-2")
    red2 = true
}

assert(x == "GLOBAL")

require red
require yellow
require red [2]
assert(x == "RED-2")
require red [1:1]
assert(x == "RED-1")

assert(yellow)
assert(red1)
assert(red2)

//  Red should not run because "require red" will load the highest qualifying version
assert(!red)
