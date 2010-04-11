/*
    Test array length truncation and growth
 */

var a = [10, 20, 30, 40, 50]
a.length = 3
assert(a.length == 3)
assert(a == "10,20,30")

a = [1,2,3,4,5]
a.length = 8
assert(a.length == 8)
assert(a == "1,2,3,4,5,,,")
