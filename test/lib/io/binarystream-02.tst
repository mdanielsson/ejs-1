/*
    Test binary stream with loopback
 */

var ba = new ByteArray
var s = BinaryStream(ba)

s.writeInteger(4000)
s.flush()
assert(ba.available == 4 || ba.available == 8)
v = s.readInteger()
assert(v == 4000)
