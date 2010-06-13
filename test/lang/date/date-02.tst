/*
    Date parsing and formatting
 */



//  Parsing
assert(Date.parseDate("August 25 2008").date == 25)
assert(Date.parseDate("August 25, 2008").date == 25)
assert((Date.parseDate("1/1/08")).year == 2008)
assert((Date.parseDate("1/1/08")).month == 0)
assert((Date.parseDate("1/1/08")).date == 1)
assert(Date.parseDate("Mon Jan 30 2008 14:30:00").year == 2008)
assert(Date.parseDate("Tuesday, 09-Nov-99 10:12:40").year == 1999)
assert(Date.parseDate("14-06-80").year == 1980)
assert(Date.parseDate("14-06-10").year == 2010)
assert(Date.parseDate("30 Jun 2008 14:30:00").date == 30)
assert(Date.parseDate("2008 30 Jun 14:30:00").date == 30)
assert(Date.parseDate("14:30 2008 30 Jun").date == 30)
assert(Date.parseDate("12/18/08 12:00:00 GMT").year == 2008)
assert(Date.parseDate("07/15/08").year == 2008)
assert(Date.parseDate("07/15/2008").year == 2008)
assert(Date.parseDate("2008-07-15 10:53:23").year == 2008)
assert(Date.parseDate("1/2/08").month == 0)

//  Parse UTC
d = Date.parseUTCDate("20:00:00")
assert(d.toUTCString() == "Thu, 01 Jan 1970 20:00:00 GMT")

d = Date.parseUTCDate("20:00:00 GMT-0100")
assert(d.toUTCString() == "Thu, 01 Jan 1970 21:00:00 GMT")

//  Should be the same using parseDate if an explicit zone is used
d = Date.parseDate("20:00:00 GMT-0100")
assert(d.toUTCString() == "Thu, 01 Jan 1970 21:00:00 GMT")

//  Parse with defaults
d = Date.parseDate("20:00:00 GMT-0100", new Date(2000,5,20))
assert(d.toUTCString() == "Tue, 20 Jun 2000 21:00:00 GMT")

assert(Date.parseUTCDate("August 25 2008").getUTCDate() == 25)
assert(Date.parseUTCDate("August 25, 2008").getUTCDate() == 25)
assert((Date.parseUTCDate("1/1/08")).getUTCFullYear() == 2008)
assert((Date.parseUTCDate("1/1/08")).getUTCMonth() == 0)
assert((Date.parseUTCDate("1/1/08")).getUTCDate() == 1)
assert(Date.parseUTCDate("Mon Jan 30 2008 14:30:00").getUTCFullYear() == 2008)
assert(Date.parseUTCDate("Tuesday, 09-Nov-99 10:12:40").getUTCFullYear() == 1999)
assert(Date.parseUTCDate("14-06-80").getUTCFullYear() == 1980)
assert(Date.parseUTCDate("14-06-10").getUTCFullYear() == 2010)
assert(Date.parseUTCDate("30 Jun 2008 14:30:00").getUTCDate() == 30)
assert(Date.parseUTCDate("2008 30 Jun 14:30:00").getUTCDate() == 30)
assert(Date.parseUTCDate("14:30 2008 30 Jun").getUTCDate() == 30)
assert(Date.parseUTCDate("12/18/08 12:00:00 GMT").getUTCFullYear() == 2008)
assert(Date.parseUTCDate("07/15/08").getUTCFullYear() == 2008)
assert(Date.parseUTCDate("07/15/2008").getUTCFullYear() == 2008)
assert(Date.parseUTCDate("2008-07-15 10:53:23").getUTCFullYear() == 2008)
assert(Date.parseUTCDate("1/2/08").getUTCMonth() == 0)

//  Formatting
d = new Date(2005, 5, 7, 10, 10)
assert(d.toString().startsWith("Tue Jun 07 2005 10:10:00 "))
assert(d.format('%m/%d/%y %H:%M:%S') == "06/07/05 10:10:00")
assert(d.format('%b %d %Y %T') == "Jun 07 2005 10:10:00")
assert(d.format('%d %b %Y %T') == "07 Jun 2005 10:10:00")

assert(d.format('%+').startsWith("Tue Jun  7 10:10:00"))
assert(d.format('%v') == " 7-Jun-2005")

if (Config.RegularExpressions) {
    assert(d.format('%d').replace(/^0/,'') == d.date)
}

//  FormatUTC
var d = Date.parseUTCDate("1/1/2010")
assert(d.formatUTC('%m/%d/%y %H:%M:%S') == "01/01/10 00:00:00")
assert(d.formatUTC('%b %d %Y %T') == "Jan 01 2010 00:00:00")
assert(d.formatUTC('%d %b %Y %T') == "01 Jan 2010 00:00:00")
