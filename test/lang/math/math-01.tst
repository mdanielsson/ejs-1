/*
   Test Math
 */



//  Test Math constants
assert(Math.E == 2.718281828459045)
assert(Math.LN2 == 0.6931471805599453)
assert(Math.LN10 == 2.302585092994046)
assert(Math.LOG2E == 1.4426950408889634)
assert(Math.LOG10E == 0.4342944819032518)
assert(Math.PI == 3.1415926535897932)
assert(Math.SQRT1_2 == 0.7071067811865476)
assert(Math.SQRT2 == 1.4142135623730951)

//  Abs
assert(Math.abs(1) == 1)
assert(Math.abs(-1) == 1)
assert(Math.abs(0) == 0)

//  Floor
assert(Math.floor(1.4) == 1)
assert(Math.floor(0.4) == 0)
assert(Math.floor(-0.4) == -1)

//  Round
assert(Math.round(1.234) == 1)
assert(Math.round(1.499) == 1)
assert(Math.round(1.5) == 2)
assert(Math.round(1.501) == 2)
assert(Math.round(-1.499) == -1)
assert(Math.round(-1.5) == -1)

//  Ceil
assert(Math.ceil(1.4) == 2)
assert(Math.ceil(0.4) == 1)
assert(Math.ceil(-0.4) == 0)

assert(Math.random() != Math.random())

assert(Math.pow(0, 1) == 0)
assert(Math.pow(0, -1) == Number.POSITIVE_INFINITY)
assert(Math.pow(1, 0) == 1)
assert(Math.pow(1, 1) == 1)
assert(Math.pow(1, 2) == 1)
assert(Math.pow(2, 0) == 1)
assert(Math.pow(2, 1) == 2)
assert(Math.pow(2, 2) == 4)
assert(Math.pow(2, 3) == 8)
assert(Math.pow(4, 3) == 64)
assert(Math.pow(-10, 2) == 100)


//  Square root
assert(Math.sqrt(0) == 0)
assert(Math.sqrt(1) == 1)
assert(Math.sqrt(2).toFixed(4) == 1.4142)
assert(Math.sqrt(4) == 2)
assert(Math.sqrt(9) == 3)
assert(Math.sqrt(100) == 10)

//  Max, min
assert(Math.max(1, 2) == 2)
assert(Math.max(-7, 2) == 2)
assert(Math.max(-7, -5) == -5)
assert(Math.min(1, 2) == 1)
assert(Math.min(-7, 2) == -7)
assert(Math.min(-5, -7) == -7)

//  Trig functions
assert(Math.acos(0).toFixed(2) == 1.57)

assert(Math.acos(.5).toFixed(2) == 1.05)
assert(Math.acos(1) == 0)

assert(Math.atan(0) == 0)
assert(Math.atan(.5).toFixed(2) == 0.46)
assert(Math.atan(1).toFixed(2) == 0.79)

assert(Math.asin(0) == 0)
assert(Math.asin(.5).toFixed(2) == 0.52)
assert(Math.asin(1).toFixed(2) == 1.57)

assert(Math.cos(0) == 1)
assert(Math.cos(.5).toFixed(2) == 0.88)
assert(Math.cos(1).toFixed(2) == 0.54)

assert(Math.sin(0) == 0)
assert(Math.sin(.5).toFixed(2) == 0.48)
assert(Math.sin(1).toFixed(2) == 0.84)

assert(Math.tan(0) == 0)
assert(Math.tan(.5).toFixed(2) == 0.55)
assert(Math.tan(1).toFixed(2) == 1.56)

assert(Math.log(0) == Number.NEGATIVE_INFINITY)
assert(Math.log(.5).toFixed(2) == -0.69)
assert(Math.log(1) == 0)

assert(Math.log(10).toFixed(1) == 2.3)
assert(Math.log(100).toFixed(1) == 4.6)

assert(Math.log10(0) == Number.NEGATIVE_INFINITY)
assert(Math.log10(.5).toFixed(1) == -0.3)
assert(Math.log10(1) == 0)
assert(Math.log10(10) == 1)
assert(Math.log10(100) == 2)

assert(Math.exp(0) == 1)
assert(Math.exp(.5).toFixed(1) == 1.6)
assert(Math.exp(1).toFixed(2) == 2.72)
assert(Math.exp(10).toFixed(2) == 22026.47)

assert(Math.atan(0,0) == 0)
assert(Math.atan(.5,.5).toFixed(2) == 0.46)
assert(Math.atan(1,1).toFixed(2) == 0.79)
