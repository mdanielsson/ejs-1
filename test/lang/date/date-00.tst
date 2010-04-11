/*
    Date tests
 */

// 

if (!this.assert) {
    this.assert = function (cond) {
        if (!cond) {
            throw "Assertion failed"
        }
    }
}
function eq(a, b) {
    actual = "" + a
    expected = "" + b
    if (actual != expected) {
        print("Assertion Failed")
        print("Actual:   " + actual)
        print("Expected: " + expected)
        assert(0)
    }
}


//  Constructor
d = new Date()
assert(d)
d = new Date(1263592610459)
eq(d.getFullYear(), 2010)
eq(d.getMonth(), 0)

d = new Date("2001/1/1")
eq(d.getFullYear(), 2001)
eq(d.getMonth(), 0)
eq(d.getDate(), 1)

d = new Date(1999, 2, 7, 11, 30, 10, 700)
eq(d.getFullYear(), 1999)
eq(d.getMonth(), 2)
eq(d.getDate(), 7)
eq(d.getHours(), 11)
eq(d.getMinutes(), 30)
eq(d.getSeconds(), 10)
eq(d.getMilliseconds(), 700)

d = new Date("1/1/2010")
assert(d.toString().contains("00:00:00"))


//  UTC
d = new Date(Date.UTC(1999, 2, 7, 11, 30, 10, 700))
if (d.getTimezoneOffset() != 0) {
    assert(d.toUTCString() != d)
}
eq(d.getUTCFullYear(), 1999)
eq(d.getUTCMonth(), 2)
eq(d.getUTCDate(), 7)
eq(d.getUTCHours(), 11)
eq(d.getUTCMinutes(), 30)
eq(d.getUTCSeconds(), 10)
eq(d.getUTCMilliseconds(), 700)


//  Set and Get
d = new Date(1999, 2, 7, 11, 30, 10, 700)
d.setFullYear(2000)
assert(d.getFullYear() == 2000)
d.setMonth(6)
assert(d.getMonth() == 6)
d.setDate(27)
assert(d.getDate() == 27)
d.setHours(23)
assert(d.getHours() == 23)
d.setMinutes(59)
assert(d.getMinutes() == 59)
d.setSeconds(45)
assert(d.getSeconds() == 45)
d.setMilliseconds(901)
assert(d.getMilliseconds() == 901)


//  getTime / setTime
d = new Date(1263592610459)
d2 = new Date
d2.setTime(1263592610459)
assert(("" + d) == ("" + d2))
eq(d2.getFullYear(), 2010)
eq(d2.getMonth(), 0)


//  Get/Set UTC
d = new Date(Date.UTC(1999, 2, 7, 11, 30, 10, 700))
d.setUTCFullYear(2000)
assert(d.getUTCFullYear() == 2000)
d.setUTCMonth(6)
assert(d.getUTCMonth() == 6)
d.setUTCDate(27)
assert(d.getUTCDate() == 27)
d.setUTCHours(23)
assert(d.getUTCHours() == 23)
d.setUTCMinutes(59)
assert(d.getUTCMinutes() == 59)
d.setUTCSeconds(45)
assert(d.getUTCSeconds() == 45)
d.setUTCMilliseconds(901)
assert(d.getUTCMilliseconds() == 901)


//  Formatting
d = new Date(1999, 2, 7, 11, 30, 10, 700)
//  Platforms provide the same output for these
assert(d.toDateString() == "Sun Mar 07 1999")
assert(d.toString().indexOf("Sun Mar 07 1999 11:30:10") == 0)
assert(d.toTimeString().indexOf("11:30:10 ") == 0)

if (this.EJSCRIPT) {
    // Platforms provide different output for these
    assert(d.toLocaleTimeString() == "11:30:10")
    assert(d.toLocaleString().indexOf("Sun, 07 Mar 1999 11:30:10") == 0)
    assert(d.toLocaleDateString().indexOf("Sun, 07 Mar 1999") == 0)
}

d = new Date(Date.UTC(1999, 2, 7, 11, 30, 10, 700))
assert(d.toUTCString() == "Sun, 07 Mar 1999 11:30:10 GMT")
if (this.EJSCRIPT) {
    assert(d.toISOString() == "1999-03-07T03:30:10.700Z")
    assert(serialize(d) == "\"1999-03-07T03:30:10.700Z\"")
}
