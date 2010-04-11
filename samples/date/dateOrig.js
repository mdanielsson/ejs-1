import System.Diagnostics;

// Define "next" method
function _next() {
	var t = this.getTime()
	var out = new Date()
	out.setTime(t + 86400000)
	return out
}
Object.prototype.next = _next


/*
 *	API tests
 */
var d = new Date()
print(d)

var date = d.getDate()
print("getDate: ", date)

var fy = d.getFullYear()
print("getFullYear: ", fy)

var hours = d.getHours()
print("getHours: ", hours)

var ms = d.getMilliseconds()
print("getMilliseconds: ", ms)

var mins = d.getMinutes()
print("getMinutes: ", mins)

var secs = d.getSeconds()
print("getSeconds: ", secs)

var t = d.getTime()
print("getTime: ", t)

var tzoffset = d.getTimezoneOffset()
print("getTimezoneOffset: ", tzoffset)

var utc = d.getUTCDate()
print("getUTCDate: ", utc)

utc = d.getUTCDay()
print("getUTCDay: ", utc)

utc = d.getUTCHours()
print("getUTCHours: ", utc)

utc = d.getUTCMilliseconds()
print("getUTCMilliseconds: ", utc)

utc = d.getUTCSeconds()
print("getUTCSeconds: ", utc)

var nextday = d.next()
print("next: " , nextday)

var d2 = new Date(Date.parse("May 5th, 1989"))
print("parse: ", d2)

d.setDate(32)
print("setDate(32): ", d)

d.setFullYear(1999)
print("setFullYear(1999): ", d)

d.setHours(19)
print("setHours(19): ", d)

d.setMinutes(19)
print("setMinutes(19): ", d)

d.setMilliseconds(99)
print("setMilliseconds(99): ", d)

d.setMonth(9)
print("setMonths(12): ", d)

d.setSeconds(12)
print("setSeconds(12): ", d)

d.setTime(1200000)
print("setTime(1200000): ", d)

d.setUTCFullYear(1999)
print("setUTCFullYear(1999): ", d)

d.setUTCHours(19)
print("setUTCHours(19): ", d)

d.setUTCMinutes(19)
print("setUTCMinutes(19): ", d)

d.setUTCMilliseconds(99)
print("setUTCMilliseconds(99): ", d)

d.setUTCMonth(2)
print("setUTCMonths(12): ", d)

d.setUTCSeconds(12)
print("setUTCSeconds(12): ", d)

print("toDateString: ", d.toDateString())
print("toLocaleDateString: ", d.toLocaleDateString())
print("toLocaleString: ", d.toLocaleString())
print("toLocaleTimeString: ", d.toLocaleTimeString())

print("\n*** Tests completed ***")
