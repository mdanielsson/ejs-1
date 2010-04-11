
/*
 *  Test comparisons with null and undefined
 */

x = 0
assert(x != null && x != undefined)

x = ""
assert(x != null && x != undefined)

x = NaN
assert(x != null && x != undefined)

x = true
assert(x != null && x != undefined)

x = false
assert(x != null && x != undefined)

x = null
assert(x == null && x == undefined)

x = undefined
assert(x == null && x == undefined)

var y
assert(y == null && y == undefined)

//  Spec throws with a reference error. Can't easily test this
// assert(z == null && z == undefined)
