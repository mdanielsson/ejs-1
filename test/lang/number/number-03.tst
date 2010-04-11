/*
   Test Number methods
 */

function eq(a, b) {
    actual = "" + a
    expected = "" + b
    if (actual != expected) {
        print("Assertion Failed: actual: " + actual + " expected: " + expected)
        assert(actual == expected)
    }
}

//  Nan and Infinity testing
n = 0.0 / 0.0
assert(n.isNaN)
assert(n.isFinite)

n = 1.0 / 0.0
assert(!n.isNaN)
assert(!n.isFinite)

n = -1.0 / 0.0
assert(!n.isNaN)
assert(!n.isFinite)

//  Byte extraction
assert(Number(1).byte == 1)
assert(Number(255).byte == 255)
assert(Number(256).byte == 0)
assert(Number(257).byte == 1)

//  Integral extraction
assert(Number(1).integral(16) == 1)
assert(Number(65535).integral(16) == 65535)
assert(Number(65536).integral(16) == 0)
assert(Number(65537).integral(16) == 1)

//  Ceil and floor
n = 1234.45
eq(n.round, "1234")
assert(n.ceil == 1235)
assert(n.floor == 1234)

n = 1234.51
eq(n.round, "1235")
assert(n.ceil == 1235)
assert(n.floor == 1234)

//  Abs
n = -1
assert(n.abs == 1)
n = 1
assert(n.abs == 1)

//  Max / Min
assert(Number(1).min(2) == 1)
assert(Number(9).min(2) == 2)
assert(Number(9).min(2) == 2)
assert(Number(-9).min(2) == -9)
assert(Number(7).min(6,8,5,100,2,200) == 2)

assert(Number(1).max(1,2) == 2)
assert(Number(9).max(2) == 9)
assert(Number(-7).max(-2) == -2)
assert(Number(-9).max(-9,-2,3,5,-10) == 5)

//  Power
assert(Number(2).power(0) == 1)
assert(Number(2).power(1) == 2)
assert(Number(2).power(2) == 4)
assert(Number(2).power(3) == 8)
assert(Number(0).power(0) == 1)
assert(Number(0).power(1) == 0)
assert(Number(1).power(2) == 1)
