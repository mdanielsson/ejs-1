/*
	Test date accessors
 */

// Property accessors 

d = new Date
assert(0 <= d.day && d.day <= 6)
assert(0 <= d.dayOfYear && d.dayOfYear <= 366)
assert(0 <= d.date && d.date <= 31)
assert(1970 <= d.fullYear && d.fullYear <= 2100)
assert(0 <= d.hours && d.hours <= 23)
assert(0 <= d.minutes && d.minutes <= 59)
assert(0 <= d.seconds && d.seconds <= 59)
assert(0 <= d.milliseconds && d.milliseconds <= 999)
assert(d.time is Number)

d = new Date("1/1/2010")
assert(d.day == 5)
assert(d.getDay() == 5)
assert(d.month == 0)
assert(d.getMonth() == 0)
assert(d.hours == 0)
assert(d.getHours() == 0)


// Misc

assert(d.elapsed >= 0)


