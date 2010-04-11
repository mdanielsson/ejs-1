/*
    Test indexOf, lastIndexOf
 */

//     0    1    2    3    4    5
a = [ 'a', 'b', 'c', 'a', 'b', 'c' ]

assert(a.indexOf('b') == 1)
assert(a.lastIndexOf('b') == 4)

assert(a.indexOf('b', 2) == 4)
assert(a.lastIndexOf('b', 4) == 4)


assert(a.indexOf('b', -1) == -1)
assert(a.indexOf('b', -2) == 4)
assert(a.indexOf('b', -3) == 4)

assert(a.lastIndexOf('b', 0) == -1)


//  Crash test dummies

a.indexOf('z')
a.lastIndexOf('z')
a.lastIndexOf(null)
a.indexOf(null, -99999999)
a.indexOf(null, 99999999)

a.lastIndexOf(0, -99999999)
a.lastIndexOf(0, 99999999)
