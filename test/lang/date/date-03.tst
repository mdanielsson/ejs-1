/*
    Date formatting checking
 */

d = new Date(2005, 5, 7, 10, 9)

function check(actual, expected) {
    if (actual != expected) {
        // print ("Mismatch \n\nActual   => \"" + actual + "\"\nExpected => \"" + expected + "\"\n")
        // breakpoint()
        throw new Error("Mismatch \n\nActual   => \"" + actual + "\"\nExpected => \"" + expected + "\"\n")
    }
}

// print(d)

//  %A    national representation of the full weekday name.
check(d.format('%A'), "Tuesday")

//  %a    national representation of the abbreviated weekday name.
check(d.format('%a'), "Tue")

//  %B    national representation of the full month name.
check(d.format('%B'), "June")

//  %b    national representation of the abbreviated month name.
check(d.format('%b'), "Jun")

//  %C    (year / 100) as decimal number; single digits are preceded by a zero.
check(d.format('%C'), 20)

//  %c    national representation of time and date.
if (Config.OS == "WIN") {
    check(d.format('%c'), "06/07/05 10:09:00")
} else {
    check(d.format('%c'), "Tue Jun  7 10:09:00 2005")
}

//  %D    is equivalent to ``%m/%d/%y''.
check(d.format('%D'), "06/07/05")

//  %d    the day of the month as a decimal number (01-31).
check(d.format('%d'), "07")

//  %E*   POSIX locale extensions. The sequences %Ec %EC %Ex %EX %Ey %EY are supposed to provide alternate 
//                      representations. NOTE: these are not available on some platforms (Windows).
check(d.format('%EC'), 20)
if (Config.OS == "WIN") {
    check(d.format('%c'), "06/07/05 10:09:00")
} else {
    check(d.format('%Ec'), "Tue Jun  7 10:09:00 2005")
}
check(d.format('%EX'), "10:09:00")
check(d.format('%Ex'), "06/07/05")
check(d.format('%EY'), 2005)
check(d.format('%Ey'), "05")

//  %e    the day of month as a decimal number (1-31); single digits are preceded by a blank.
check(d.format('%e'), " 7")

//  %F    is equivalent to ``%Y-%m-%d''.
check(d.format('%F'), "2005-06-07")

//  MOB -- hide this (week based year) - Not supported on Windows or VxWorks
//  %G    a year as a decimal number with century. This year is the one that contains the greater part of
//                      the week (Monday as the first day of the week).
//  MOB/WIN check(d.format('%G'), "2005")

//  MOB -- hide this (week based year) - Not supported on Windows or VxWorks
//  %g    the same year as in ``%G'', but as a decimal number without century (00-99).
//  MOB/WIN check(d.format('%g'), "05")

//  %H    the hour (24-hour clock) as a decimal number (00-23).
check(d.format('%H'), 10)

//  %h    the same as %b.
check(d.format('%h'), "Jun")

//  %I    the hour (12-hour clock) as a decimal number (01-12).
check(d.format('%I'), 10)

//  %j    the day of the year as a decimal number (001-366). Note: this range is different to that of
//                      the dayOfYear property which is 0-365.
check(d.format('%j'), 158)

//  %k    the hour (24-hour clock) as a decimal number (0-23); single digits are preceded by a blank.
check(d.format('%k'), 10)

//  %l    the hour (12-hour clock) as a decimal number (1-12); single digits are preceded by a blank.
check(d.format('%l'), 10)

//  %M    the minute as a decimal number (00-59).
check(d.format('%M'), "09")

//  %m    the month as a decimal number (01-12).
check(d.format('%m'), "06")

//  %n    a newline.
check(d.format('%n'), "\n")

//  %O*   POSIX locale extensions. The sequences %Od %Oe %OH %OI %Om %OM %OS %Ou %OU %OV %Ow %OW %Oy are 
//        supposed to provide alternate representations. Additionly %OB implemented to represent alternative 
//        names (used standalone, without day mentioned). NOTE: these are not available on some platforms
check(d.format('%Od'), "07")
check(d.format('%Oe'), " 7")
check(d.format('%OH'), "10")
check(d.format('%OI'), "10")
check(d.format('%Om'), "06")
check(d.format('%OM'), "09")
check(d.format('%OS'), "00")
check(d.format('%Ou'), "2")
check(d.format('%OU'), "23")

//  MOB -- remove V
//  MOB/WIN check(d.format('%OV'), "23")

check(d.format('%Ow'), "2")
check(d.format('%OW'), "23")
check(d.format('%Oy'), "05")

//  %P    Lower case national representation of either "ante meridiem" or "post meridiem" as appropriate.
//  MOB - BUG on MAC - not present
if (Config.OS != "MACOSX") {
    check(d.format('%P'), "am")
}

//  %p    national representation of either "ante meridiem" or "post meridiem" as appropriate.
check(d.format('%p'), "AM")

//  %R    is equivalent to ``%H:%M''.
check(d.format('%R'), "10:09")

//  %r    is equivalent to ``%I:%M:%S %p''.
check(d.format('%r'), "10:09:00 AM")

//  %S    the second as a decimal number (00-60).
check(d.format('%S'), "00")

//  %s    the number of seconds since the Epoch, UTC (see mktime(3)).
check(d.format('%s'), "1118164140")

//  %T    is equivalent to ``%H:%M:%S''.
check(d.format('%T'), "10:09:00")

//  %t    a tab.
check(d.format('%t'), "\t")

//  %U    the week number of the year (Sunday as the first day of the week) as a decimal number (00-53).
check(d.format('%U'), "23")

//  %u    the weekday (Monday as the first day of the week) as a decimal number (1-7).
check(d.format('%u'), "2")

//  MOB -- hide
//  %V    the week number of the year (Monday as the first day of the week) as a decimal
//        number (01-53).  If the week containing January 1 has four or more days in the new year, then it
//        is week 1; otherwise it is the last week of the previous year, and the next week is week 1.
//  MOB/WIN check(d.format('%V'), "23")

//  %v    is equivalent to ``%e-%b-%Y''.
check(d.format('%v'), " 7-Jun-2005")

//  %W    the week number of the year (Monday as the first day of the week) as a decimal number (00-53).
check(d.format('%W'), "23")

//  %w    the weekday (Sunday as the first day of the week) as a decimal number (0-6).
check(d.format('%w'), "2")

//  %X    national representation of the time.
check(d.format('%X'), "10:09:00")

//  %x    national representation of the date.
check(d.format('%x'), "06/07/05")

//  %Y    the year with century as a decimal number.
check(d.format('%Y'), "2005")

//  %y    the year without century as a decimal number (00-99).
check(d.format('%y'), "05")

//  %Z    the time zone name.
if (Config.OS == "WIN") {
    check(d.format('%Z'), "Pacific Daylight Time")
} else {
    check(d.format('%Z'), "PDT")
}

//  MOB -- windows says -800
//  %z    the time zone offset from UTC; a leading plus sign stands for east of UTC, a minus
//        sign for west of UTC, hours and minutes follow with two digits each and no delimiter between them
//        (common form for RFC 822 date headers).
check(d.format('%z'), "-0700")

//  %+    national representation of the date and time (the format is similar to that produced by date(1)).
//        This format is platform dependent.
if (Config.OS == "WIN") {
    check(d.format('%+'), "Tue Jun 07 10:09:00 Pacific Daylight Time 2005")
} else {
    check(d.format('%+'), "Tue Jun  7 10:09:00 PDT 2005")
}

//  %%    Literal percent.
check(d.format('%%'), "%")
