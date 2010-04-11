/*
 *  Text stream
 */

var b: ByteArray
var t: TextStream 

//  writeLine 

b = new ByteArray(1000)
t = new TextStream(b)
t.writeLine("line 1", "line 2", "line 3")
assert(b == "line 1\r\nline 2\r\nline 3\r\n" || b == "line 1\nline 2\nline 3\n")

//  read

b = new ByteArray(1000)
t = new TextStream(b)
b.write("Line one\nLine 2\nLine 3") 
dest = new ByteArray(1000)
count = t.read(dest)
assert(count == 22)
assert(dest.readString() == "Line one\nLine 2\nLine 3") 

b = new ByteArray(1000)
t = new TextStream(b)
b.write("Line one\nLine two\nLine three") 
a = t.readLines()

//	Layered stream access

var s: TextStream = Path("file.tmp").openTextStream("wt")
s.writeLine("Hello")
s.writeLine("World")
s.flush()
s.close()

s = Path("file.tmp").openTextStream("rt")
assert(s.readLine() == "Hello")
assert(s.readLine() == "World")

s.close()
rm("file.tmp")
