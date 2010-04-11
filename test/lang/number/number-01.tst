/*
   Test number conversions to string
 */


var n

//  Test toString
n = 7
assert(n.toString() == "7")

//  Test typed toString
var y: Number = 7
assert(y.toString() == "7")

//  Yes this is right - inexact representation of .1
n = .1 * .1
assert(n == 0.010000000000000002)


function eq(a, b) {
    actual = "" + a
    expected = "" + b
    if (actual != expected) {
        print("Assertion Failed: actual: " + actual + " expected: " + expected)
        assert(actual == expected)
    }
}


//  toString
n = 0.0
eq(n, "0")

n = -0.0
eq(n, "0")

n = Number.POSITIVE_INFINITY
eq(n, "Infinity")

n = Number.NEGATIVE_INFINITY
eq(n, "-Infinity")

n = Number.MAX_VALUE
eq(n, "1.7976931348623157e+308")

n = Number.MIN_VALUE
eq(n, "5e-324")

n = Number.NaN
eq(n, "NaN")

n = 12345.67891234
eq(n, "12345.67891234")
n = 0.001234
eq(n, "0.001234")

n = 0.000000001234
eq(n, "1.234e-9")

n = -0.001234
eq(n, "-0.001234")

n = -0.000000001234
eq(n, "-1.234e-9")

n = -7.1234e100
eq(n, "-7.1234e+100")

n = 123456789123456.000000012345
eq(n, "123456789123456")



//  toFixed
n = 12345.67891234
eq(n.toFixed(), "12346")

eq(n.toFixed(1), "12345.7")
eq(n.toFixed(6), "12345.678912")
eq(n.toFixed(10), "12345.6789123400")

n = 12345.0
eq(n.toFixed(10), "12345.0000000000")

n = 0.0012
eq(n.toFixed(6), "0.001200")

n = 0.000000012345
eq(n.toFixed(6), "0.000000")

n = 0.00000012345
eq(n.toFixed(7), "0.0000001")

n = 0.000000012345
eq(n.toFixed(7), "0.0000000")

n = 0.0000000012345
eq(n.toFixed(7), "0.0000000")

n = 123456789123456.000000012345
eq(n.toFixed(6), "123456789123456.000000")
eq(n.toFixed(20), "123456789123456.00000000000000000000")

n = -123456789123456.000000012345
eq(n.toFixed(6), "-123456789123456.000000")


// toExponential
n = 77.123456789
eq(n.toExponential(), "7.7123456789e+1")
eq(n.toExponential(1), "7.7e+1")
eq(n.toExponential(2), "7.71e+1")
eq(n.toExponential(4), "7.7123e+1")

n = -7.123456789
eq(n.toExponential(), "-7.123456789e+0")

n = 0.0071
eq(n.toExponential(), "7.1e-3")

n = 12345.67891234
eq(n.toExponential(), "1.234567891234e+4")

n = 12345.0
eq(n.toExponential(), "1.2345e+4")

n = -123456789123456.000000012345
eq(n.toExponential(6), "-1.234568e+14")


//  toPrecision
n = 5.12345678;
eq(n.toPrecision(), "5.12345678")

eq(n.toPrecision(1), "5")
eq(n.toPrecision(2), "5.1")
eq(n.toPrecision(4), "5.123")

n = 0.0001234
eq(n.toPrecision(), "0.0001234")
eq(n.toPrecision(1), "0.0001")
eq(n.toPrecision(2), "0.00012")
eq(n.toPrecision(4), "0.0001234")

n = -0.0001234
eq(n.toPrecision(), "-0.0001234")
eq(n.toPrecision(1), "-0.0001")
eq(n.toPrecision(2), "-0.00012")
