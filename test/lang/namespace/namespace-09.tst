
/*
 *  Test namespace qualified property access
 */

var o = {}

o.public::abc = 1
assert(o.public::abc == 1)

o."public"::n = 2
assert(o."public"::n == 2)

o["n"] = 3
assert(o["n"] == 3)

o."public"::["" + "n"] = 4
assert(o."public"::["" + "n"] == 4)

o."public"::["n"] = 1
assert(o."public"::["n"] == 1)

var name = "abc"
o."public"::[name] = 1
assert(o."public"::[name] == 1)
assert(o[name] == 1)
assert(o["abc"] == 1)

namespace red
red::["x"] = 1
assert(red::["x"] == 1)

"public"::["x"] = 1
assert("public"::["x"] == 1)

public::["x"] = 1
assert(public::["x"] == 1)
