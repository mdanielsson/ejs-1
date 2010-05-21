/*
    Test year ranges in date constructors
 */

d = new Date(3000, 0, 1)
assert(d.year == 3000)
d = new Date("3000")
assert(d.year == 3000)

d = new Date(2050, 0, 1)
assert(d.year == 2050)

d = new Date(2000, 0, 1)
assert(d.year == 2000)

d = new Date(1972, 0, 1)
assert(d.year == 1972)

d = new Date(1971, 0, 1)
assert(d.year == 1971)

d = new Date(1970, 0, 1)
assert(d.year == 1970)

d = new Date(1969, 0, 1)
assert(d.year == 1969)

d = new Date(1902, 0, 1)
assert(d.year == 1902)
d = new Date("1902")
assert(d.year == 1902)

d = new Date(1901, 0, 1)
assert(d.year == 1901)

d = new Date(1900, 0, 1)
assert(d.year == 1900)

d = new Date(1899, 0, 1)
assert(d.year == 1899)

d = new Date(1600, 0, 1)
assert(d.year == 1600)

d = new Date(1600, 0, 1)
assert(d.year == 1600)

d = new Date(1000, 0, 1)
assert(d.year == 1000)
d = new Date("1000")
assert(d.year == 1000)

d = new Date(100, 0, 1)
assert(d.year == 100)

d = Date(90, 0, 1)
assert(d.year == 1990)

d = Date(0, 0, 1)
assert(d.year == 1900)

d = Date(-5, 0, 1)
assert(d.year == -5)

d = new Date("1000")
assert(d.year == 1000)

d = new Date("100")
assert(d.year == 100)
