/*
 *  File size for open files was failing
 */
let datafile = "regress.dat"

rm(datafile)
file = new File(datafile)
file.open("w")
file.write("12345")
file.close()

f = new File(datafile)
s1 = f.size
f = new File(datafile)
f.open({mode: "r"})
s2 = f.size
f.close()

assert(s1 == s2)
rm(datafile)
