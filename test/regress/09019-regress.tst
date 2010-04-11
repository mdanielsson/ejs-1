/*
 *  Test reading an empty file
 */

p = Path(".test.tmp")
p.write()
assert(p.readString() == "")
p.remove()
