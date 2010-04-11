/*
   Test Number constants
 */

//  This is right - unbelievable but correct.
assert(Number.NaN != (0.0 / 0.0))

assert(Number.MAX_VALUE == 1.7976931348623157e+308)

assert(Number.MIN_VALUE == 5e-324)

assert(Number.NEGATIVE_INFINITY == -1.0 / 0.0)

assert(Number.POSITIVE_INFINITY == 1.0 / 0.0)

n = 0.0 / 0.0
assert(n.isNaN)
