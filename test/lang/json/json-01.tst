
/*
 *  JSON tests
 */

s = "a"
assert(JSON.stringify(s) == '"a"')

s = 77
assert(JSON.stringify(s) == '77')

s = -1
assert(JSON.stringify(s) == '-1')

s = true
assert(JSON.stringify(s) == 'true')

s = false
assert(JSON.stringify(s) == 'false')

s = "a b c \"d e f\""
assert(JSON.stringify(s) == '"a b c \\"d e f\\""')

o = {
    a: "a b c \"d e f\"",
    "one two": "zzz",
}

tmp = JSON.stringify(o)
o2 = JSON.parse(tmp)
assert(o.a == o2.a)
assert(o["one two"] == o2["one two"])
