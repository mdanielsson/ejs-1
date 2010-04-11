
/*
 *  Test delete
 */

var o = {}
o.a = 7
assert(o.a == 7)
delete o.a
assert(o.a == undefined)

var a = [1, 2, 3]
delete a[0]

o = { "blue"::name: 1, }
assert(o."blue"::["name"] == 1)
delete o."blue"::["name"]
assert(o."blue"::["name"] == undefined)

a = [1, 2, 3]

assert(a.length == 3)
assert(a[0] == 1)
delete a[0]
assert(a[0] == undefined)

delete a[6 - 7 + 1]
assert(a == ",2,3")

var x = 7
assert(x == 7)
assert(global["x"] == 7)
delete x
assert(global["x"] == undefined)

o = {}
o.x = 7
assert(o.x == 7)
delete o["" + "x"]
assert(o.x == undefined)

namespace red
o = {}
o.red::["a" + "b"] = 77
assert(o.red::["a" + "b"] == 77)
delete o.red::["a" + "b"]
assert(o.red::["a" + "b"] == undefined)

o = {}
o."red"::["a" + "b"] = 77
assert(o."red"::["a" + "b"] == 77)
delete o."red"::["a" + "b"]
assert(o."red"::["a" + "b"] == undefined)
